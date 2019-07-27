#pragma once

#include <cgraphics/Mesh.hpp>
#include <cgraphics/Texture.hpp>

#include <filesystem>
#include <map>

class ResourceManager
{
private:
    std::map<std::filesystem::path, Mesh> meshes;
    std::map<std::filesystem::path, Texture> textures;

    ResourceManager() = default;
    ~ResourceManager() = default;
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    static ResourceManager &get_instance();
    Mesh get_mesh(const std::filesystem::path &mesh);
    Texture get_texture(const std::filesystem::path &texture);

    int get_meshes_count() const;
    int get_textures_count() const;
};
