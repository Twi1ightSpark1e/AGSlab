#include <cgraphics/Mesh.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

namespace fs = std::experimental::filesystem;

void Mesh::load(const fs::path &file)
{
    static const std::regex  vertex_matcher(R"(^v (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+))");
    static const std::regex texture_matcher(R"(^vt (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+))");
    static const std::regex  normal_matcher(R"(^vn (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+))");
    static const std::regex    face_matcher(R"(^f (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+))");
    std::smatch regex_groups;

    std::vector<std::array<float,3>> vertices, normals;
    std::vector<std::array<float,2>> textures;
    std::vector<Vertex> final_vertices;

    std::ifstream obj(file);
    std::string tmp;
    while (std::getline(obj, tmp))
    {
        if (std::regex_match(tmp, regex_groups, vertex_matcher))
        {
            auto &arr = vertices.emplace_back();
            arr[0] = std::stof(regex_groups[1].str());
            arr[1] = std::stof(regex_groups[2].str());
            arr[2] = std::stof(regex_groups[3].str());
        }
        else if (std::regex_match(tmp, regex_groups, texture_matcher))
        {
            auto &arr = textures.emplace_back();
            arr[0] = std::stof(regex_groups[1].str());
            arr[1] = std::stof(regex_groups[2].str());
        }
        else if (std::regex_match(tmp, regex_groups, normal_matcher))
        {
            auto &arr = normals.emplace_back();
            arr[0] = std::stof(regex_groups[1].str());
            arr[1] = std::stof(regex_groups[2].str());
            arr[2] = std::stof(regex_groups[3].str());
        }
        else if (std::regex_match(tmp, regex_groups, face_matcher))
        {
            for (uint i = 0; i < 3; i++)
            {
                auto &vertex = final_vertices.emplace_back();
                vertex.position = vertices[std::stof(regex_groups[i * 3 + 1].str()) - 1];
                vertex.texture  = textures[std::stof(regex_groups[i * 3 + 2].str()) - 1];
                vertex.normal   =  normals[std::stof(regex_groups[i * 3 + 3].str()) - 1];
            }
        }
    }
    obj.close();
    vertex_count = final_vertices.size();
    std::cout << "vertex_count = " << vertex_count << std::endl;

    glGenBuffers(1, &vbo_index);
    std::cout << "vbo_index = " << vbo_index << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * final_vertices.size(), final_vertices.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao_index);
    std::cout << "vao_index = " << vao_index << std::endl;
    glBindVertexArray(vao_index);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, texture)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Mesh::render()
{
    glBindVertexArray(vao_index);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glBindVertexArray(0);
}