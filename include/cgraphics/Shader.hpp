#pragma once

#include <cgraphics/Extensions.hpp>

#include <glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// КЛАСС ДЛЯ РАБОТЫ С ШЕЙДЕРОМ
class Shader
{
private:
    // вершинный шейдер
    GLuint vsh;
    // фрагментный шейдер
    GLuint fsh;
    // шейдерная программа (шейдер)
    GLuint program;

    void load_shader(GLuint &id, GLenum type, std::vector<std::string> text, bool debug);

public:
    // загрузить вершинный шейдер
    void load_vertex_shader(std::string filename, bool debug = true);
    // загрузить фрагментный шейдер
    void load_fragment_shader(std::string filename, bool debug = true);
    // слинковать шейдерную программу
    void link(bool debug = true);

    // активизировать шейдер (сделать текущим)
    void activate();
    // отключить шейдер (использовать нулевую шейдерную программу)
    static void deactivate();

    // получение индекса атрибут-переменной
    int get_attrib_location(std::string name)
    {
        return glGetAttribLocation(program, name.c_str());
    };

    // запись вектора из 4-х комопнент в uniform-переменную
    void set_uniform_vec4(std::string name, glm::vec4 value)
    {
        int k = glGetUniformLocation (program, name.c_str());
        if (k < 0) return;
        glUseProgram (program);
        glUniform4fv (k, 1, glm::value_ptr(value));
    }
};