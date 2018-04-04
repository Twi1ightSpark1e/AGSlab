#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Material
{
private:
    glm::vec4 ambient, diffuse, specular;
    GLfloat shininess;
public:
    Material() = default;
    Material(const glm::vec4 &ambient, const glm::vec4 &diffuse, const glm::vec4 &specular, const GLfloat &shininess)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->shininess = shininess;
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

    void set_shininess(const GLfloat &shininess)
    {
        this->shininess = shininess;
    }
    GLfloat get_shininess() const
    {
        return shininess;
    }

    bool operator==(const Material &b) const
    {
        return ((ambient == b.ambient) &&
            (diffuse == b.diffuse) && 
            (specular == b.specular) && 
            (shininess == b.shininess));
    }
    bool operator!=(const Material &b) const
    {
        return ((ambient != b.ambient) ||
            (diffuse != b.diffuse) || 
            (specular != b.specular) || 
            (shininess != b.shininess));
    }
};
