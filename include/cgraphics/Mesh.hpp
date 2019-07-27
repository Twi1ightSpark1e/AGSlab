#pragma once

#include <GL/glew.h>

#include <array>
#include <filesystem>

#pragma pack(push, 0)
struct Vertex
{
    std::array<GLfloat,3> position, normal;
    std::array<GLfloat,2> texture;
};
#pragma pack(pop)

class Mesh
{
private:
    GLuint vao_index, vbo_index;
    int    vertex_count;
public:
    Mesh() = default;
    explicit Mesh(const std::filesystem::path &file)
    {
        load(file);
    }

    void load(const std::filesystem::path &file);
    void load_rectangle();
    void render() const;
};
