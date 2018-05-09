#include <cgraphics/GraphicObject.hpp>

#include <cmath>

namespace fs = std::experimental::filesystem;

GraphicObject::GraphicObject()
{
    model = glm::mat4(
        glm::vec4(1, 0, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 0, 1)
    );

    is_initialized = false;
}

bool GraphicObject::initialized() const
{
    return is_initialized;
}

void GraphicObject::set_id(const int &id)
{
    this->id = id;
    is_initialized = true;
}

int GraphicObject::get_id() const
{
    return id;
}

void GraphicObject::set_mesh(const fs::path &mesh)
{
    this->mesh = mesh;
}

fs::path GraphicObject::get_mesh() const
{
    return mesh;
}

void GraphicObject::set_material(const Material &material)
{
    this->material = material;
}

Material GraphicObject::get_material() const
{
    return material;
}

void GraphicObject::set_texture(const fs::path &texture)
{
    this->texture = texture;
}

fs::path GraphicObject::get_texture() const
{
    return texture;
}

void GraphicObject::set_position(const glm::vec4 &position)
{
    model[3] = position;
}

void GraphicObject::set_position(const glm::vec3 &position)
{
    model[3] = glm::vec4(position, 1);
}

void GraphicObject::set_rotation(const float &degree)
{
    model[0][0] =  std::cos(glm::radians(degree));
    model[0][2] =  std::sin(glm::radians(degree));

    model[2][0] = -std::sin(glm::radians(degree));
    model[2][2] =  std::cos(glm::radians(degree));
}

glm::mat4 GraphicObject::get_model() const
{
    return model;
}

glm::vec3 GraphicObject::get_position() const
{
    return glm::vec3(model[3][0], model[3][1], model[3][2]);
}

void GraphicObject::set_aabb(const std::array<float, 3> &aabb)
{
    this->aabb = aabb;
}

std::array<float, 3> GraphicObject::get_aabb() const
{
    return aabb;
}

void GraphicObject::set_object_type(const unsigned char &type)
{
    object_type = type;
}

unsigned char GraphicObject::get_object_type() const
{
    return object_type;
}

bool GraphicObject::operator==(const GraphicObject &b) const
{
    return ((material == b.material) &&
        (model == b.model) &&
        (id == b.id) &&
        (mesh == b.mesh) &&
        (aabb == b.aabb));
}

bool GraphicObject::operator!=(const GraphicObject &b) const
{
    return !((*this) == b);
}
