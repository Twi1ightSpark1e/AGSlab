#include <cgraphics/ResourceManager.hpp>

#include <algorithm>

namespace fs = std::experimental::filesystem;

ResourceManager &ResourceManager::get_instance()
{
    static ResourceManager rm;
    return rm;
}

Mesh ResourceManager::get_mesh(const fs::path &mesh)
{
    auto search = meshes.find(mesh);
    if (search != meshes.end())
    {
        return search->second;
    }
    auto m = Mesh(mesh);
    meshes.emplace(mesh, m);
    return m;
}

Texture ResourceManager::get_texture(const fs::path &texture)
{
    auto search = textures.find(texture);
    if (search != textures.end())
    {
        return search->second;
    }
    auto t = Texture(texture);
    textures.emplace(texture, t);
    return t;
}

int ResourceManager::get_meshes_count() const
{
    return meshes.size();
}

int ResourceManager::get_textures_count() const
{
    return textures.size();
}
