#include <cgraphics/ResourceManager.hpp>

#include <algorithm>

namespace fs = std::experimental::filesystem;

ResourceManager &ResourceManager::get_instance()
{
    static ResourceManager rm;
    return rm;
}

Mesh ResourceManager::get_mesh(const fs::path &mesh) const
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