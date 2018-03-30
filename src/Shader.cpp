#include <cgraphics/Extensions.hpp>
#include <cgraphics/Shader.hpp>

#include <GL/freeglut.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>

void Shader::load_shader(GLuint &id, GLenum type, const std::vector<std::string> &text, bool debug)
{
    auto [strings,lengths] = Extensions::vector_to_array(text);
    if (debug)
    {
        std::cout << "Creating and compiling shader" << std::endl;
        for (uint i = 0; i < text.size(); i++)
        {
            std::cout << strings[i];
        }
    }
    id = glCreateShader(type);
    glShaderSource(id, text.size(), strings, lengths);
    glCompileShader(id);
    GLint is_compiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);
    if(is_compiled == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(id, max_length, &max_length, &error_log[0]);

        std::cerr << "Error while compiling shader!" << std::endl;
        std::cerr << error_log.data() << std::endl;

        glDeleteShader(id);
    }
    if (debug)
    {
        std::cout << "Shader compiled successfully" << std::endl;
    }
    Extensions::remove_array(strings, text.size());
    delete[] lengths;
}

void Shader::load_vertex_shader(const std::experimental::filesystem::path &file, bool debug)
{
    if (debug)
    {
        std::cout << "Loading " << file << std::endl;
    }
    auto shader = Extensions::read_file(file);
    load_shader(vsh, GL_VERTEX_SHADER, shader, debug);
}

void Shader::load_fragment_shader(const std::experimental::filesystem::path &file, bool debug)
{
    if (debug)
    {
        std::cout << "Loading " << file << std::endl;
    }
    auto shader = Extensions::read_file(file);
    load_shader(fsh, GL_FRAGMENT_SHADER, shader, debug);
}

void Shader::link(bool debug)
{
    if (debug)
    {
        std::cout << "Creating program" << std::endl;
    }
    program = glCreateProgram();
    if (debug)
    {
        std::cout << "Attaching shaders and linking program" << std::endl;
    }
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);
    glLinkProgram(program);

    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if(is_linked == GL_FALSE)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<GLchar> error_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &error_log[0]);

        std::cerr << "Error while linking program!" << std::endl;
        std::cerr << error_log.data() << std::endl;

        glDeleteProgram(program);
        glDeleteShader(vsh);
        glDeleteShader(fsh);
        exit(1);
    }
    if (debug)
    {
        std::cout << "Program compiled successfully" << std::endl;
    }
    glDetachShader(program, vsh);
    glDetachShader(program, fsh);
}

void Shader::activate()
{
    glUseProgram(program);
}

void Shader::deactivate()
{
    glUseProgram(0);
}

int Shader::get_attrib_location(const std::string &name)
{
    return glGetAttribLocation(program, name.c_str());
}

int Shader::get_uniform_location(const std::string &name)
{
    auto found_element = uniforms.find(name);
    if (found_element != uniforms.end())
    {
        return found_element->second;
    }
    int location = glGetUniformLocation(program, name.c_str());
    if (location >= 0)
    {
        uniforms.emplace(name, location);
    }
    return location;
}

void Shader::set_uniform_vec4(const std::string &name, glm::vec4 value)
{
    int location = get_uniform_location(name);
    if (location >= 0)
    {
        glUseProgram(program);
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::set_uniform_mat4(const std::string &name, glm::mat4 value)
{
    int location = get_uniform_location(name);
    if (location >= 0)
    {
        glUseProgram(program);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::set_uniform_float(const std::string &name, float value)
{
    int location = get_uniform_location(name);
    if (location >= 0)
    {
        glUseProgram(program);
        glUniform1f(location, value);
    }
}