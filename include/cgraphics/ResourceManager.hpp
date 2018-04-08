#pragma once

#include <cgraphics/Mesh.hpp>
#include <cgraphics/Texture.hpp>

#include <experimental/filesystem>
#include <map>

class ResourceManager
{
private:
    std::map<std::experimental::filesystem::path, Mesh> meshes;
    std::map<std::experimental::filesystem::path, Texture> textures;

    ResourceManager() = default;
    ~ResourceManager() = default;
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    static ResourceManager &get_instance();
    Mesh get_mesh(const std::experimental::filesystem::path &mesh);
    Texture get_texture(const std::experimental::filesystem::path &texture);
};