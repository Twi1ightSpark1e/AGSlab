#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>

#include <experimental/filesystem>
#include <string>
#include <map>

#include <pugixml.hpp>

#include <ohf/tcp/Socket.hpp>

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
    std::map<int, GraphicObject> objects;
    pugi::xml_document xml;
    std::experimental::filesystem::path base_path;
    ohf::tcp::Socket sock;
    unsigned short transaction_id;

    GraphicObject create_graphic_object(const std::string &name);
    void simulate_mouse();
    void simulate_keyboard(double delta_s);
public:
    Scene() noexcept {} // NOLINT cause linter suggests replace {} with default, but it's not valid
    ~Scene() noexcept;

    void init(const std::experimental::filesystem::path &base_path);
    void simulate(double seconds);
    void draw();

    Camera &get_camera();
};
