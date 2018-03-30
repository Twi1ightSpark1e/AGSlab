#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <tuple>

class Light
{
private:
    glm::vec4 position, ambient, diffuse, specular;
public:
    Light() = default;
    Light(const glm::vec4 &position, const glm::vec4 &ambient, const glm::vec4 &diffuse, const glm::vec4 &specular)
    {
        this->position = position;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    void set_position(const glm::vec4 &position)
    {
        this->position = position;
    }
    glm::vec4 get_position() const
    {
        return position;
    }

    void set_ambient(const glm::vec4 &ambient)
    {
        this->ambient = ambient;
    }
    glm::vec4 get_ambient() const
    {
        return ambient;
    }

    void set_diffuse(const glm::vec4 &diffuse)
    {
        this->diffuse = diffuse;
    }
    glm::vec4 get_diffuse() const
    {
        return diffuse;
    }

    void set_specular(const glm::vec4 &specular)
    {
        this->specular = specular;
    }
    glm::vec4 get_specular() const
    {
        return specular;
    }
};