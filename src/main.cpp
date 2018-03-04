#include <cstdio>

#include <cgraphics/Shader.hpp>

#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std::string_literals;

// используемый шейдер (пока только один)
Shader shader;

// переменные для вывода объекта (прямоугольника из двух треугольников)
GLuint vao_index  = 0;    // индекс VAO-буфера
GLuint vbo_index  = 0;    // индекс VBO-буфера
int vertex_count  = 0;    // количество вершин

// инициализация переменных для вывода объекта
void init_object ()
{
    // создание и заполнение VBO
    glGenBuffers (1, &vbo_index);
    glBindBuffer (GL_ARRAY_BUFFER, vbo_index);
    GLfloat verteces [] = {
        -0.5,0.5,
        -0.5,-0.5,
        0.5,0.5,
        0.5,0.5,
        -0.5,-0.5,
        0.5,-0.5
    };
    glBufferData (GL_ARRAY_BUFFER, sizeof(verteces), verteces, GL_STATIC_DRAW);

    // создание VAO
    glGenVertexArrays (1, &vao_index);
    glBindVertexArray (vao_index);
    // заполнение VAO
    glBindBuffer (GL_ARRAY_BUFFER, vbo_index);
    int k = shader.get_attrib_location("vPosition"s);
    glVertexAttribPointer (k, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(k);
    // "отвязка" буфера VAO, чтоб случайно не испортить
    glBindVertexArray(0);

    // указание количество вершин
    vertex_count = 6;
}

// функция вызывается при перерисовке окна
// в том числе и принудительно, по командам glutPostRedisplay
void display()
{
    // отчищаем буфер цвета
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable (GL_DEPTH_TEST);

    // активируем шейдер
    shader.activate();

    // установка uniform-переменных
    glm::vec4 offset = glm::vec4(0.5, 0.5, 0.0, 0.0);
    shader.set_uniform_vec4 ("Offset"s, offset);
    glm::vec4 color = glm::vec4 (1.0, 0.0, 0.0, 1.0);
    shader.set_uniform_vec4 ("Color"s, color);

    // вывод объекта
    glBindVertexArray (vao_index);
    glDrawArrays (GL_TRIANGLES, 0, 6); 

    // смена переднего и заднего буферов
    glutSwapBuffers();
};

// функция, вызываемая при изменении размеров окна
void reshape(int w,int h)
{
    // установить новую область просмотра, равную всей области окна
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
};

// функция вызывается когда процессор простаивает, т.е. максимально часто
void simulation()
{
    //ПЕРЕРИСОВАТЬ ОКНО
    glutPostRedisplay();
};

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
    glutCreateWindow("laba_01");

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
    shader.load_vertex_shader ("../shaders/example.vsh"s);
    shader.load_fragment_shader ("../shaders/example.fsh"s);
    shader.link();
    // инициализация объекта для вывода
    init_object();

    // устанавливаем функцию, которая будет вызываться для перерисовки окна
    glutDisplayFunc(display);
    // устанавливаем функцию, которая будет вызываться при изменении размеров окна
    glutReshapeFunc(reshape);
    // устанавливаем функцию которая вызывается всякий раз, когда процессор простаивает
    glutIdleFunc(simulation);
    // основной цикл обработки сообщений ОС
    glutMainLoop();
    return 0;
};

