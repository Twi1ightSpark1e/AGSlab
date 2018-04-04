#include <array>
#include <cstdio>
#include <chrono>
#include <csignal>
#include <experimental/filesystem>
#include <iostream>

#include <cgraphics/Extensions.hpp>
#include <cgraphics/CameraController.hpp>
#include <cgraphics/Scene.hpp>
#include <cgraphics/RenderManager.hpp>

#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::experimental::filesystem;

// сцена
Scene scene;
// Камера
CameraController& camera_controller = CameraController::get_instance();
fs::path meshes_folder;

// функция вызывается при перерисовке окна
// в том числе и принудительно, по командам glutPostRedisplay
void display()
{
    RenderManager::get_instance().start();
    scene.draw();
    RenderManager::get_instance().finish();
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
    static auto time_base = time_prev;
    static auto frames = 0;

    auto time_current = high_resolution_clock::now();
    auto time_from_base = duration_cast<milliseconds>(time_current - time_base).count();
    auto delta_mcs = duration_cast<microseconds>(time_current - time_prev).count();
    auto delta_s = double(delta_mcs) / 1'000'000;
    time_prev = time_current;
    frames++;

    scene.simulate(delta_s);

    if (time_from_base >= 500)
    {
        glutSetWindowTitle(("FPS: " + std::to_string(int(frames * 1000. / time_from_base)) + 
            "; UBO Updates = " + std::to_string(RenderManager::get_update_count())).c_str());
        time_base = time_current;
        frames = 0;
    }
    //ПЕРЕРИСОВАТЬ ОКНО
    glutPostRedisplay();
}

void speckey(int key, int state)
{
    switch (key)
    {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            camera_controller.set_arrow_state(key, state);
            return;
    }
}

void speckey_down(int key, int, int)
{
    speckey(key, GLUT_DOWN);
}

void speckey_up(int key, int, int)
{
    speckey(key, GLUT_UP);
}

void motion(int x, int y)
{
    camera_controller.set_mouse_state('x', x);
    camera_controller.set_mouse_state('y', y);
}

void mouse(int button, int state, int x, int y)
{
    switch (button)
    {
        case 0:
        case 1:
        case 2:
            camera_controller.set_mouse_state(button, state);
            motion(x, y);
            return;
        case 3: // колёсико вверх
            scene.get_camera().zoom(0.05);
            return;
        case 4: // колёсико вниз
            scene.get_camera().zoom(-0.05);
            return;
        default:
            return;
    }
}

void sigint_handler(int)
{
    glutLeaveMainLoop();
}

int main(int argc,char **argv)
{
    // инициализация библиотеки GLUT
    glutInit(&argc,argv);
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
    glutCreateWindow("laba_04");

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
    auto exec_path = Extensions::resolve_dots(fs::current_path() / std::string(argv[0]));
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
    // функция, которая регистрирует перемещение мыши с зажатой кнопкой
    glutMotionFunc(motion);
    // функция, которая вызывается каждый раз, когда нажимается кнопка мыши, или крутится колесо
    glutMouseFunc(mouse);
    // функции обработки специальных кнопок
    glutSpecialFunc(speckey_down);
    glutSpecialUpFunc(speckey_up);
    // убираем повторение кнопок, т.к. мы регистрируем моменты нажатия и отпускания
    glutSetKeyRepeat(GL_FALSE);
    // обработчик сигнала SIGINT для корректного выхода из программы
    signal(SIGINT, sigint_handler);
    // основной цикл обработки сообщений ОС
    glutMainLoop();
    return 0;
}
