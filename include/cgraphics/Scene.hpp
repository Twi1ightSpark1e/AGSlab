#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>

#include <experimental/filesystem>
#include <string>
#include <vector>

#include <pugixml.hpp>

class Scene
{
private:
    Camera camera;
    Light  light;
    std::vector<GraphicObject> objects;
    pugi::xml_document xml;
    std::experimental::filesystem::path base_path;

    GraphicObject create_graphic_object(const std::string &name);
    void simulate_mouse();
    void simulate_keyboard(double delta_s);
public:
    Scene() noexcept {} // NOLINT cause linter suggests replace {} with default, but it's not valid

    void init(const std::experimental::filesystem::path &base_path);
    void simulate(double seconds);
    void draw();

    Camera &get_camera();
};
