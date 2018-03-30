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