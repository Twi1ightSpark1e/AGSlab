#include <cstdio>
#include <chrono>
#include <csignal>
#include <experimental/filesystem>
#include <iostream>

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/CameraController.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/Mesh.hpp>

#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// используемый шейдер (пока только один)
Shader shader;
// Камера
Camera camera;
CameraController& camera_controller = CameraController::get_instance();
Mesh mesh_drug_store, mesh_tree;

// функция вывода плоскости
void draw_cube(Shader &shader) {
    using namespace std::string_literals;
// переменные для вывода объекта (прямоугольника из двух треугольников)
    static GLuint vao_index = 0;    // индекс VAO-буфера
    static GLuint vbo_index = 0;    // индекс VBO-буфера
    static int    vertex_count = 0;  // количество вершин
    static bool   init = true;
    if (init) {
        // создание и заполнение VBO
        glGenBuffers(1, &vbo_index);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
        GLfloat Verteces[] = {
            // передняя грань
            -0.5, +0.5, +0.5,
            -0.5, -0.5, +0.5,
            +0.5, +0.5, +0.5,
            +0.5, +0.5, +0.5,
            -0.5, -0.5, +0.5,
            +0.5, -0.5, +0.5,
            // задняя грань
            +0.5, +0.5, -0.5,
            +0.5, -0.5, -0.5,
            -0.5, +0.5, -0.5,
            -0.5, +0.5, -0.5,
            +0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5,
            // правая грань
            +0.5, -0.5, +0.5,
            +0.5, -0.5, -0.5,
            +0.5, +0.5, +0.5,
            +0.5, +0.5, +0.5,
            +0.5, -0.5, -0.5,
            +0.5, +0.5, -0.5,
            // левая грань
            -0.5, +0.5, +0.5,
            -0.5, +0.5, -0.5,
            -0.5, -0.5, +0.5,
            -0.5, -0.5, +0.5,
            -0.5, +0.5, -0.5,
            -0.5, -0.5, -0.5,
            // верхняя грань
            -0.5, +0.5, -0.5,
            -0.5, +0.5, +0.5,
            +0.5, +0.5, -0.5,
            +0.5, +0.5, -0.5,
            -0.5, +0.5, +0.5,
            +0.5, +0.5, +0.5,
            // нижняя грань
            -0.5, -0.5, +0.5,
            -0.5, -0.5, -0.5,
            +0.5, -0.5, +0.5,
            +0.5, -0.5, +0.5,
            -0.5, -0.5, -0.5,
            +0.5, -0.5, -0.5
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(Verteces), Verteces, GL_STATIC_DRAW);
        // создание VAO
        glGenVertexArrays(1, &vao_index);
        glBindVertexArray(vao_index);
        // заполнение VAO
        glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
        int k = shader.get_attrib_location("vPosition"s);
        glVertexAttribPointer(k, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(k);
        // "отвязка" буфера VAO, чтоб случайно не испортить
        glBindVertexArray(0);
        // указание количество вершин
        vertex_count = 6*6;
        init = false;
    }
    glBindVertexArray(vao_index);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

// функция вызывается при перерисовке окна
// в том числе и принудительно, по командам glutPostRedisplay
void display()
{
    using namespace std::string_literals;

    static auto model = glm::mat4(
        glm::vec4(1, 0, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4()
    );
    static std::tuple<glm::vec4, glm::vec4> cubes[] { // первый вектор - позиция, второй вектор - цвет
        std::make_tuple(glm::vec4( 5, 0,  5, 1), glm::vec4 (1, 0, 0, 1)),
        std::make_tuple(glm::vec4(10, 0,  5, 1), glm::vec4 (0, 1, 0, 1)),
        std::make_tuple(glm::vec4(-2, 0,  2, 1), glm::vec4 (0, 0, 1, 1)),
        std::make_tuple(glm::vec4( 2, 0,  2, 1), glm::vec4 (1, 0, 1, 1)),
        std::make_tuple(glm::vec4( 0, 2,  0, 1), glm::vec4 (0, 0, 0, 1)),
    };

    // отчищаем буфер цвета
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable (GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto projection = camera.get_projection_matrix();
    auto view = camera.get_view_matrix();

    // активируем первый шейдер
    shader.activate();
    // инициализируем uniform-переменные
    shader.set_uniform_mat4("ProjectionMatrix"s, projection);
    bool mesh_store_shown = false, mesh_tree_shown = false;
    for (auto cube : cubes)
    {
        model[3] = std::get<0>(cube);
        shader.set_uniform_mat4("ModelViewMatrix"s, view * model);
        shader.set_uniform_vec4("Color"s, std::get<1>(cube));
        // выводим объект
        if (!mesh_store_shown)
        {
            mesh_drug_store.render();
            //std::cout << "mesh shown" << std::endl;
            mesh_store_shown = true;
        }
        else if (!mesh_tree_shown)
        {
            mesh_tree.render();
            mesh_tree_shown = true;
        }
        else draw_cube(shader);
    }

    glutSwapBuffers();
}

// функция, вызываемая при изменении размеров окна
void reshape(int w,int h)
{
    // установить новую область просмотра, равную всей области окна
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    camera.set_projection_matrix(glm::radians(45.0), float(w) / float(h), .1, 1000);
};

void simulate_mouse()
{
    static int mouse_x, mouse_y, prev_button_state = GLUT_UP;

    if ((prev_button_state == GLUT_UP) && 
        (camera_controller.get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        mouse_x = camera_controller.get_mouse_state('x');
        mouse_y = camera_controller.get_mouse_state('y');
        prev_button_state = GLUT_DOWN;
    }
    else if ((prev_button_state == GLUT_DOWN) && 
        (camera_controller.get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        float new_x = camera_controller.get_mouse_state('x');
        float new_y = camera_controller.get_mouse_state('y');
        camera.rotate((new_x - mouse_x) / 300, (new_y - mouse_y) / 300);
        mouse_x = new_x;
        mouse_y = new_y;
    }
    else if (camera_controller.get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_UP)
    {
        prev_button_state = GLUT_UP;
    }
}

void simulate_keyboard(double delta_s)
{
    if (camera_controller.get_arrow_state(GLUT_KEY_LEFT) == GLUT_DOWN)
    {
        camera.move_oxz(0, -delta_s);
    }
    if (camera_controller.get_arrow_state(GLUT_KEY_UP) == GLUT_DOWN)
    {
        camera.move_oxz(delta_s, 0);
    }
    if (camera_controller.get_arrow_state(GLUT_KEY_RIGHT) == GLUT_DOWN)
    {
        camera.move_oxz(0, delta_s);
    }
    if (camera_controller.get_arrow_state(GLUT_KEY_DOWN) == GLUT_DOWN)
    {
        camera.move_oxz(-delta_s, 0);
    }
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

    simulate_mouse();
    simulate_keyboard(delta_s);

    if (time_from_base >= 500)
    {
        glutSetWindowTitle(("FPS: " + std::to_string(int(frames * 1000. / time_from_base))).c_str());
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
            camera.zoom(0.05);
            return;
        case 4: // колёсико вниз
            camera.zoom(-0.05);
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
    namespace fs = std::experimental::filesystem;

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
    glutCreateWindow("laba_02");

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
    // parent_path два раза, так как fs::path считает имя исполняемого файла за имя директории
    auto base_path = exec_path.parent_path().parent_path();
    // шейдеры, используемые в данной лабораторной работе, лежат в папке shaders
    // и имеют общее название cube
    fs::path shader_basename = base_path / "shaders" / "cube";
    // модели лежат в папке meshes
    fs::path meshes_folder = base_path / "meshes";
    // загружаем модель аптеки
    mesh_drug_store.load(meshes_folder / "buildings" / "drug_store.obj");
    mesh_tree.load(meshes_folder / "natures" / "big_tree.obj");
    // загружаем вершинный и фрагментный шейдеры
    shader.load_vertex_shader (shader_basename.replace_extension(".vsh"), false);
    shader.load_fragment_shader (shader_basename.replace_extension(".fsh"), false);
    // связываем шейдеры в программу
    shader.link(false);

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
};

