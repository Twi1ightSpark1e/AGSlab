#pragma once

#include <cgraphics/Extensions.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <map>

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

    std::map<std::string, int> uniforms;

    void load_shader(GLuint &id, GLenum type, const std::vector<std::string> &text, bool debug);
    int get_uniform_location(const std::string &name);

public:
    // загрузить вершинный шейдер
    void load_vertex_shader(const std::string &filename, bool debug = true);
    // загрузить фрагментный шейдер
    void load_fragment_shader(const std::string &filename, bool debug = true);
    // слинковать шейдерную программу
    void link(bool debug = true);

    // активизировать шейдер (сделать текущим)
    void activate();
    // отключить шейдер (использовать нулевую шейдерную программу)
    static void deactivate();

    // получение индекса атрибут-переменной
    int get_attrib_location(const std::string &name);

    // запись вектора из 4-х комопнент в uniform-переменную
    void set_uniform_vec4(const std::string &name, glm::vec4 value);
    // запись матрицы 4х4 в uniform-переменную
    void set_uniform_mat4(const std::string &name, glm::mat4 value);
};