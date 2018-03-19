#include <cgraphics/Mesh.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <regex>

namespace fs = std::experimental::filesystem;

void Mesh::load(const fs::path &file)
{
    // регулярные выражения для поиска нужных строк
    // строка с текстурными координатами
    static const std::regex       texture_matcher(R"(^vt (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+))");
    // строка с вершинами
    static const std::regex          face_matcher(R"(^f (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+))");
    // строка с нормалями или с координатами вершин
    static const std::regex vertex_normal_matcher(R"(^(?:vn|v) (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+) (-?\d*\.{0,1}\d+)$)");
    // место хранения захваченных групп
    std::smatch regex_groups;

    std::vector<std::array<float,3>> vertices, normals;
    std::vector<std::array<float,2>> textures;
    std::vector<Vertex> final_vertices;

    std::ifstream obj(file);
    std::string tmp;
    while (std::getline(obj, tmp))
    {
        if (std::regex_match(tmp, regex_groups, vertex_normal_matcher))
        {
            // строка, начинающаяся с 'vn' хранит в себе нормали, иначе координаты вершин
            auto &arr = (tmp.find("vn") == 0) ? normals.emplace_back() : vertices.emplace_back();
            arr[0] = std::stof(regex_groups[1].str());
            arr[1] = std::stof(regex_groups[2].str());
            arr[2] = std::stof(regex_groups[3].str());
        }
        else if (std::regex_match(tmp, regex_groups, texture_matcher))
        {
            // строка хранит в себе текстурные координаты
            auto &arr = textures.emplace_back();
            arr[0] = std::stof(regex_groups[1].str());
            arr[1] = std::stof(regex_groups[2].str());
        }
        else if (std::regex_match(tmp, regex_groups, face_matcher))
        {
            // в строке находится по три вершины
            for (uint i = 0; i < 3; i++)
            {
                auto &vertex = final_vertices.emplace_back();
                vertex.position = vertices[std::stoi(regex_groups[i * 3 + 1].str()) - 1];
                vertex.texture  = textures[std::stoi(regex_groups[i * 3 + 2].str()) - 1];
                vertex.normal   =  normals[std::stoi(regex_groups[i * 3 + 3].str()) - 1];
            }
        }
    }
    obj.close();
    vertex_count = final_vertices.size();
    // если модель уже была загружена - удалим её
    if (vao_index != 0)
    {
        glDeleteVertexArrays(1, &vao_index);
        vao_index = 0;
    }
    if (vbo_index != 0)
    {
        glDeleteBuffers(1, &vbo_index);
        vbo_index = 0;
    }
    // загружаем вершины в память видеокарты
    // генерируем VBO
    glGenBuffers(1, &vbo_index);
    // указываем, что сгенерированный буфер хранит в себе массив данных
    glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
    // загружаем вершины
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * final_vertices.size(), final_vertices.data(), GL_STATIC_DRAW);
    // генерируем VAO
    glGenVertexArrays(1, &vao_index);
    // начинаем работу с созданным VAO
    glBindVertexArray(vao_index);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_index);
    // указываем свойства аттрибутов шейдера
    // позиция вершины
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, position)));
    // нормаль вершины
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, normal)));
    // текстурная координата вершины
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<GLvoid*>(offsetof(Vertex, texture)));
    // включаем аттрибуты
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    // отвязываем VAO чтобы не испортить
    glBindVertexArray(0);
}

void Mesh::render()
{
    // начинаем работу с созданным VAO
    glBindVertexArray(vao_index);
    // рисуем вершины
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    // отвязываем VAO чтобы не испортить
    glBindVertexArray(0);
}
