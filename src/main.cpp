#include <array>
#include <cstdio>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>

#include <cgraphics/Extensions.hpp>
#include <cgraphics/InputManager.hpp>
#include <cgraphics/Scene.hpp>
#include <cgraphics/RenderManager.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <boost/format.hpp>

#include <GLFW/glfw3.h>

#include <IL/ilu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

// сцена
Scene scene;
// Камера
InputManager& input_manager = InputManager::get_instance();
// Текущий FPS
int current_fps;

// функция вызывается при перерисовке окна
void display(GLFWwindow *window)
{
    using namespace std::chrono;
    using namespace std::string_literals;

    static auto time_base = high_resolution_clock::now();
    static auto frames = 0;
    static auto &render_manager = RenderManager::get_instance();

    scene.draw(window);

    auto time_current = high_resolution_clock::now();
    auto time_from_base = duration_cast<milliseconds>(time_current - time_base).count();
    frames++;

    std::string optimization;
    switch (scene.get_enabled_optimizations())
    {
        case Scene::Optimization::None:
            optimization = "None"s;
            break;
        case Scene::Optimization::Frustum:
            optimization = "Frustum"s;
            break;
        case Scene::Optimization::FrustumLoD:
            optimization = "Frustum + LoD"s;
            break;
    }

    std::string msaa_mode;
    switch (render_manager.get_multisampling_mode())
    {
        case RenderManager::MSAAMode::None:
            msaa_mode = "NoMSAA"s;
            break;
        case RenderManager::MSAAMode::X2:
            msaa_mode = "MSAAx2"s;
            break;
        case RenderManager::MSAAMode::X4:
            msaa_mode = "MSAAx4"s;
            break;
        case RenderManager::MSAAMode::X8:
            msaa_mode = "MSAAx8"s;
            break;
        default:
            msaa_mode = "Wrong MSAA";
            return;
    }

    std::string post_processing;
    switch (render_manager.get_post_processing())
    {
        case RenderManager::PostProcessingMode::Simple:
            post_processing = "Simple";
            break;
        case RenderManager::PostProcessingMode::Sepia:
            post_processing = "Sepia";
            break;
        case RenderManager::PostProcessingMode::Grey:
            post_processing = "Grey";
            break;
        default:
            post_processing = "Wrong PP";
            break;
    }

    auto new_title = boost::format("[%1% FPS][%2%][%3%][optimization: %4%][objects: %5%]")
        % current_fps
        % msaa_mode
        % post_processing
        % optimization
        % render_manager.get_objects_count();
    glfwSetWindowTitle(window, new_title.str().c_str());
    if (time_from_base >= 500)
    {
        current_fps = int(frames * 1000. / time_from_base);
        time_base = time_current;
        frames = 0;
    }
}

// функция, вызываемая при изменении размеров окна
void reshape(GLFWwindow *window, int w, int h)
{
    // установить новую область просмотра, равную всей области окна
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    scene.get_camera().set_projection_matrix(glm::radians(45.0), float(w) / float(h), .1, 1000);
    RenderManager::get_instance().initialize_framebuffers(w, h);
}

// функция вызывается когда процессор простаивает, т.е. максимально часто
void simulation()
{
    using namespace std::chrono;

    static auto time_prev = high_resolution_clock::now();

    auto time_current = high_resolution_clock::now();
    auto delta_mcs = duration_cast<microseconds>(time_current - time_prev).count();
    auto delta_s = double(delta_mcs) / 1'000'000;
    time_prev = time_current;
    scene.simulate(delta_s);
}

int main(int argc,char **argv)
{
    // инициализируем графическую библиотеку
    if (!glfwInit()) {
        std::cerr << "Cannot initialize graphical library" << std::endl;
        return 1;
    }

    // устанавливаем параметры окна
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // создаем окно
    GLFWwindow *window = glfwCreateWindow(800, 600, "laba_08", NULL, NULL);
    if (!window) {
        std::cerr << "Cannot create window" << std::endl;
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, reshape);
    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);

    // Инициализация DevIL
    ilInit();
    iluInit();

    // инициализация GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "GLEW error: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return 3;
    }

    // вывод текущей версии OpenGL
    std::cout << "OpenGL Version = " << glGetString(GL_VERSION) << std::endl << std::endl;

    // определение пути с исполняемым файлом
    auto exec_path = Extensions::resolve_dots((argv[0][0] == '/') || (argv[0][1] == ':')
        ? fs::path(argv[0])
        : (fs::current_path() / std::string(argv[0]))
    );
    // поднимаемся на одну директорию вверх, так как сборка идёт в папке build
    // parent_path два раза, так как сначала мы получаем родителя исполняемого файла,
    // а затем - родителя родителя :^)
    auto base_path = exec_path.parent_path().parent_path();
    // инициализируем сцену
    scene.init(base_path);

    // устанавливаем обработчики мыши и клавиатуры
    input_manager.set_handlers(window, scene.get_camera());
    // добавляем события клавиатуры:
    //  1. если нажата q - выход из программы
    input_manager.set_key_handler(GLFW_KEY_Q, [&window] {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    });
    //  2. если нажата кнопка Tab - вывести статистику
    input_manager.set_key_handler(GLFW_KEY_TAB, [] {
        auto eye = scene.get_camera().get_eye(), center = scene.get_camera().get_center();
        std::cout << "                   Statistics" << std::endl;
        std::cout << "                 Current FPS: " << current_fps << std::endl;
        std::cout << "         Current UBO updates: " << RenderManager::get_update_count() << std::endl;
        std::cout << "        Loaded meshes amount: " << ResourceManager::get_instance().get_meshes_count() << std::endl;
        std::cout << "      Loaded textures amount: " << ResourceManager::get_instance().get_textures_count() << std::endl;
        auto camera_position = boost::format("(%1%; %2%; %3%)") % eye.x % eye.y % eye.z;
        std::cout << "             Camera position: " << camera_position << std::endl;
        auto camera_center = boost::format("(%1%; %2%; %3%)") % center.x % center.y % center.z;
        std::cout << " Where the camera is looking: " << camera_center << std::endl << std::endl;
    });
    //  3. если нажата кнопка 1 - сменить режим сглаживания
    input_manager.set_key_handler(GLFW_KEY_1, [] {
        RenderManager::get_instance().next_multisampling_mode();
    });
    //  4. если нажата кнопка 2 - сменить режим отображения AABB
    input_manager.set_key_handler(GLFW_KEY_2, [] {
        RenderManager::get_instance().next_post_processing();
    });
    //  5. если нажата кнопка 3 - сменить режим Frustum Culling
    input_manager.set_key_handler(GLFW_KEY_3, [] {
        scene.next_optimization();
    });

    // цикл рендера
    while (!glfwWindowShouldClose(window)) {
        simulation(); // симуляция содержимого
        display(window); // вывод содержимого
        glfwPollEvents(); // обрабатываем ожидающие события
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
