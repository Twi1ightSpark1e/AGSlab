#pragma once

#include <glm/glm.hpp>

#include <cgraphics/Material.hpp>
#include <cgraphics/Mesh.hpp>
#include <cgraphics/Texture.hpp>

#include <experimental/filesystem>

class GraphicObject
{
private:
    int id;
    std::experimental::filesystem::path mesh, texture;
    Material material;
    glm::mat4 model;
public:
    GraphicObject();

    void set_id(const int &id);
    int get_id() const;

    void set_mesh(const std::experimental::filesystem::path &mesh);
    std::experimental::filesystem::path get_mesh() const;

    void set_texture(const std::experimental::filesystem::path &texture);
    std::experimental::filesystem::path get_texture() const;

    void set_material(const Material &material);
    Material get_material() const;

    void set_position(const glm::vec4 &position);
    void set_position(const glm::vec3 &position);
    void set_rotation(const float &degree);
    glm::mat4 get_model() const;

    bool operator==(const GraphicObject &b) const;
    bool operator!=(const GraphicObject &b) const;
};
