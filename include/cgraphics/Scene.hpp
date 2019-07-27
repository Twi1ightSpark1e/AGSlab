#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>
#include <cgraphics/SkyBox.hpp>
#include <cgraphics/NetProtocol.hpp>

#include <filesystem>
#include <string>
#include <map>

#include <pugixml.hpp>

class Scene
{
private:
    #pragma pack(push, 1)
    struct NetworkHeader
    {
        unsigned short transaction_id, frame_number, frame_count;
        unsigned int data_length;
        unsigned char function_id;
    };
    struct GameObjectDescription
    {
        unsigned int object_id;
        unsigned char object_type;
        float x, y, z;
        float rotation;
        char model_name[16];
        char reserved[59];
    };
    #pragma pack(pop)

    Camera camera;
    Light  light;
    SkyBox skybox;
    std::map<int, GraphicObject> objects;
    pugi::xml_document xml;
    std::filesystem::path base_path;
    NetProtocol protocol;
    bool culling_enabled = true, lod_enabled = true;

    void frustum_culling(std::vector<std::reference_wrapper<GraphicObject>> &render_objects);
    void level_of_detail(std::vector<std::reference_wrapper<GraphicObject>> &render_objects);
    void create_graphic_object(const std::string &name, GraphicObject &out);
    void simulate_mouse();
    void simulate_keyboard(double delta_s);
public:
    enum Optimization { None, Frustum, FrustumLoD };

    Scene() noexcept {}

    void init(const std::filesystem::path &base_path);
    void simulate(double seconds);
    void draw();

    Optimization get_enabled_optimizations() const;
    void next_optimization();

    Camera &get_camera();
};
