#include <array>
#include <cstdio>
#include <chrono>
#include <csignal>
#include <experimental/filesystem>
#include <iostream>

#include <cgraphics/Extensions.hpp>
#include <cgraphics/InputManager.hpp>
#include <cgraphics/Scene.hpp>
#include <cgraphics/RenderManager.hpp>

#include <GL/freeglut.h>

#include <IL/ilu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::experimental::filesystem;

// сцена
Scene scene;
// Камера
InputManager& input_manager = InputManager::get_instance();

// функция вызывается при перерисовке окна
// в том числе и принудительно, по командам glutPostRedisplay
void display()
{
    using namespace std::chrono;

    static auto time_base = high_resolution_clock::now();;
    static auto frames = 0;

    RenderManager::get_instance().start();
    scene.draw();
    RenderManager::get_instance().finish();

    auto time_current = high_resolution_clock::now();
    auto time_from_base = duration_cast<milliseconds>(time_current - time_base).count();
    frames++;
    if (time_from_base >= 500)
    {
        glutSetWindowTitle(("FPS: " + std::to_string(int(frames * 1000. / time_from_base)) + 
            "; UBO Updates = " + std::to_string(RenderManager::get_update_count())).c_str());
        time_base = time_current;
        frames = 0;
    }
}

// функция, вызываемая при изменении размеров окна
void reshape(int w,int h)
{
    // установить новую область просмотра, равную всей области окна
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    scene.get_camera().set_projection_matrix(glm::radians(45.0), float(w) / float(h), .1, 1000);
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
    //ПЕРЕРИСОВАТЬ ОКНО
    glutPostRedisplay();
}

void sigint_handler(int)
{
    glutLeaveMainLoop();
}

int main(int argc,char **argv)
{
    // инициализация библиотеки GLUT
    glutInit(&argc,argv);
    // обработка закрытия окна GLUT
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    // инициализация дисплея (формат вывода)
    glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE );
    // требования к версии OpenGL (версия 3.3 без поддержки обратной совместимости)
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    // устанавливаем верхний левый угол окна
    glutInitWindowPosition(300,100);
    // устанавливаем размер окна
    glutInitWindowSize(800,600);
    // создание окна
    glutCreateWindow("laba_05");

    // Инициализация DevIL
    ilInit();
    iluInit();

    //инициализация GLEW 
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "GLEW error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    // определение текущей версии OpenGL
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

    // устанавливаем функцию, которая будет вызываться для перерисовки окна
    glutDisplayFunc(display);
    // устанавливаем функцию, которая будет вызываться при изменении размеров окна
    glutReshapeFunc(reshape);
    // устанавливаем функцию которая вызывается всякий раз, когда процессор простаивает
    glutIdleFunc(simulation);
    // устанавливаем обработчики мыши и клавиатуры
    input_manager.set_handlers(scene.get_camera());
    // убираем повторение кнопок, т.к. мы регистрируем моменты нажатия и отпускания
    glutSetKeyRepeat(GL_FALSE);
    // обработчик сигнала SIGINT для корректного выхода из программы
    std::signal(SIGINT, sigint_handler);
    // основной цикл обработки сообщений ОС
    glutMainLoop();
    return 0;
}
