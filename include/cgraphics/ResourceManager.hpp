#pragma once

#include <cgraphics/Mesh.hpp>

#include <experimental/filesystem>
#include <map>

class ResourceManager
{
private:
    mutable std::map<std::experimental::filesystem::path, Mesh> meshes;

    ResourceManager() = default;
    ~ResourceManager() = default;
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    static ResourceManager &get_instance();
    Mesh get_mesh(const std::experimental::filesystem::path &mesh) const;
};