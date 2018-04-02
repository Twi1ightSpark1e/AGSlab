#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>

#include <vector>
#include <experimental/filesystem>

class RenderManager
{
private:
    Shader shader;
    Camera camera;
    Light  light;
    std::vector<GraphicObject> objects;

    RenderManager() = default;
    ~RenderManager() = default;
public:
    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&) = delete;

    static RenderManager& get_instance()
    {
        static RenderManager manager;
        return manager;
    }

    void init(const std::experimental::filesystem::path& vsh, const std::experimental::filesystem::path& fsh);
    void start();

    void set_camera(const Camera &camera);
    void set_light(const Light &light);
    void add_to_queue(const GraphicObject &object);

    void finish();
};
