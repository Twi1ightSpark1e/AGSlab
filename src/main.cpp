#include <cstdio>
#include <chrono>

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>

#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std::string_literals;
using namespace std::chrono;

// используемый шейдер (пока только один)
Shader shader;

// функция вывода плоскости
void draw_cube(Shader &shader) {
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
        glVertexAttribPointer(k, 3, GL_FLOAT, GL_TRUE, 0, 0);
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
    static auto model = glm::mat4(
        glm::vec4(1, 0, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4()
    );
    static std::tuple<glm::vec4, glm::vec4> cubes[] { // первый вектор - позиция, второй вектор - цвет
        std::make_tuple(glm::vec4( 2, 0,  2, 1), glm::vec4 (1, 0, 0, 1)),
        std::make_tuple(glm::vec4(-2, 0, -2, 1), glm::vec4 (0, 1, 0, 1)),
        std::make_tuple(glm::vec4(-2, 0,  2, 1), glm::vec4 (0, 0, 1, 1)),
        std::make_tuple(glm::vec4( 2, 0, -2, 1), glm::vec4 (1, 0, 1, 1)),
        std::make_tuple(glm::vec4( 0, 2,  0, 1), glm::vec4 (0, 0, 0, 1)),
    };

    // отчищаем буфер цвета
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable (GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto projection = Camera::get_instance().get_projection_matrix();
    auto view = Camera::get_instance().get_view_matrix();

    // активируем первый шейдер
    shader.activate();
    // инициализируем uniform-переменные
    shader.set_uniform_mat4("ProjectionMatrix"s, projection);
    for (auto cube : cubes)
    {
        model[3] = std::get<0>(cube);
        shader.set_uniform_mat4("ModelViewMatrix"s, view * model);
        shader.set_uniform_vec4("Color"s, std::get<1>(cube));
        // выводим объект
        draw_cube(shader);
    }
    shader.deactivate();

    glutSwapBuffers();
}

// функция, вызываемая при изменении размеров окна
void reshape(int w,int h)
{
    // установить новую область просмотра, равную всей области окна
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    Camera::get_instance().set_projection_matrix(glm::radians(45.0), float(w) / float(h), .1, 1000);
};

// функция вызывается когда процессор простаивает, т.е. максимально часто
void simulation()
{
    static auto time_prev = high_resolution_clock::now();
    static auto time_base = time_prev;
    static auto frames = 0;
    auto time_current = high_resolution_clock::now();
    auto time_from_base = duration_cast<milliseconds>(time_current - time_base).count();
    auto delta_mcs = duration_cast<microseconds>(time_current - time_prev).count();
    auto delta_s = double(delta_mcs) / 1'000'000;
    time_prev = time_current;
    frames++;

    if (time_from_base >= 500)
    {
        glutSetWindowTitle(("FPS: " + std::to_string(int(frames * 1000. / time_from_base))).c_str());
        time_base = time_current;
        frames = 0;
    }
    //std::cout << delta_s << std::endl;
    //Camera::get_instance().rotate(delta_s, 0);
    //Camera::get_instance().zoom(delta_s);
    //Camera::get_instance().move_oxz(delta_s, 0);
    //ПЕРЕРИСОВАТЬ ОКНО
    glutPostRedisplay();
}

void key_down(unsigned char key, int x, int y)
{
    std::cout << (int)key << " down" << std::endl;
}

void key_release(unsigned char key, int x, int y)
{
    std::cout << (int)key << " released" << std::endl;
}

void speckey_down(int key, int x, int y)
{
    std::cout << key << " down" << std::endl;
}

void speckey_up(int key, int x, int y)
{
    std::cout << key << " released" << std::endl;
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
    glutCreateWindow("laba_02");

    //инициализация GLEW 
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Glew error: %s\n", glewGetErrorString(err));
        return 1;
    }
    // определение текущей версии OpenGL
    printf("OpenGL Version = %s\n\n", glGetString(GL_VERSION));

    // загрузка шейдера
    shader.load_vertex_shader ("../shaders/cube.vsh"s, false);
    shader.load_fragment_shader ("../shaders/cube.fsh"s, false);
    shader.link();

    // устанавливаем функцию, которая будет вызываться для перерисовки окна
    glutDisplayFunc(display);
    // устанавливаем функцию, которая будет вызываться при изменении размеров окна
    glutReshapeFunc(reshape);
    // устанавливаем функцию которая вызывается всякий раз, когда процессор простаивает
    glutIdleFunc(simulation);

    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_release);
    glutSpecialFunc(speckey_down);
    glutSpecialUpFunc(speckey_up);
    glutSetKeyRepeat(GL_FALSE);
    // основной цикл обработки сообщений ОС
    glutMainLoop();
    return 0;
};

