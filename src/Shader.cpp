#include <cgraphics/Shader.hpp>

void Shader::load_shader(GLuint &id, GLenum type, const std::vector<std::string> &text, bool debug)
{
    auto [strings,lengths] = Extensions::vector_to_array(text);
    if (debug)
    {
        std::cout << "Creating and compiling shader" << std::endl;
        for (uint i = 0; i < text.size(); i++)
        {
            std::cout << strings[i] << std::endl;
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
        exit(1);
    }
    if (debug)
    {
        std::cout << "Shader compiled successfully" << std::endl;
    }
    Extensions::remove_array(strings, text.size());
    delete[] lengths;
}

void Shader::load_vertex_shader(const std::string &filename, bool debug)
{
    if (debug)
    {
        std::cout << "Loading " << filename << std::endl;
    }
    auto shader = Extensions::read_file(filename);
    load_shader(vsh, GL_VERTEX_SHADER, shader, debug);
}

void Shader::load_fragment_shader(const std::string &filename, bool debug)
{
    if (debug)
    {
        std::cout << "Loading " << filename << std::endl;
    }
    auto shader = Extensions::read_file(filename);
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