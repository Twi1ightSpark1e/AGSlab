#pragma once

#include <cgraphics/Mesh.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <GL/glew.h>

#include <IL/ilu.h>

#include <filesystem>
#include <vector>

class SkyBox
{
private:
    struct TextureInfo
    {
        ILint width, height;
        std::vector<ILbyte> data;
    };

    GLuint texture_id;
    std::filesystem::path mesh;

    TextureInfo load_image(const std::filesystem::path &path);
    void load_texture(const int &target, const std::filesystem::path &path);
public:
    SkyBox() = default;
    SkyBox(const std::filesystem::path &base_path, const std::string &extension) noexcept
    {
        load(base_path, extension);
    }

    void load(const std::filesystem::path &base_path, const std::string &extension) noexcept;

    void render(GLenum texture_unit) noexcept;
};
