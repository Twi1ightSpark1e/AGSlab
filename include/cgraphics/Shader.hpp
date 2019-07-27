#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <filesystem>
#include <map>
#include <vector>

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
    enum Type
    {
        SimplePostProcessing,
        SepiaPostProcessing,
        GreyPostProcessing,
        DirectLight,
        SkyBox,
        BoundingBox,
        Amount
    };

    GLuint get_program() const;
    // загрузить вершинный шейдер
    void load_vertex_shader(const std::filesystem::path &file, bool debug = true);
    // загрузить фрагментный шейдер
    void load_fragment_shader(const std::filesystem::path &file, bool debug = true);
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
    // запись вещественного числа в uniform-переменную
    void set_uniform_float(const std::string &name, float value);
    // запись целого числа в uniform-переменную
    void set_uniform_int(const std::string &name, int value);
};
