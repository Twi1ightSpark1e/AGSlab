#pragma once

#include <glm/glm.hpp>

#include <cgraphics/Material.hpp>
#include <cgraphics/Mesh.hpp>
#include <cgraphics/Texture.hpp>

#include <array>
#include <filesystem>

class GraphicObject
{
private:
    int id;
    std::filesystem::path mesh, texture;
    Material material;
    glm::mat4 model;
    std::array<float, 3> aabb;
    unsigned char object_type;
    bool is_initialized;
public:
    GraphicObject();

    bool initialized() const;

    void set_id(const int &id);
    int get_id() const;

    void set_mesh(const std::filesystem::path &mesh);
    std::filesystem::path get_mesh() const;

    void set_texture(const std::filesystem::path &texture);
    std::filesystem::path get_texture() const;

    void set_material(const Material &material);
    Material get_material() const;

    void set_position(const glm::vec4 &position);
    void set_position(const glm::vec3 &position);
    void set_rotation(const float &degree);
    glm::mat4 get_model() const;
    glm::vec3 get_position() const;

    void set_aabb(const std::array<float, 3> &aabb);
    std::array<float, 3> get_aabb() const;

    void set_object_type(const unsigned char &type);
    unsigned char get_object_type() const;

    bool operator==(const GraphicObject &b) const;
    bool operator!=(const GraphicObject &b) const;
};
