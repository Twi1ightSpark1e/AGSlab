#pragma once

#include <GL/glew.h>

#include <filesystem>

class Texture
{
private:
    GLuint texture_id;
public:
    Texture() = default;
    Texture(const std::filesystem::path &path, bool flip = false) noexcept
    {
        load(path, flip);
    }

    void load(const std::filesystem::path &path, bool flip = false);
    void apply(GLenum texture_unit = GL_TEXTURE0) const;
};
