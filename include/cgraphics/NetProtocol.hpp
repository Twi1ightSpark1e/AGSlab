#pragma once

#include <cgraphics/GraphicObject.hpp>

#include <ohf/tcp/Socket.hpp>

#include <glm/glm.hpp>

#include <array>
#include <vector>

class NetProtocol
{
private:
    #pragma pack(push, 1)
    struct NetworkHeader
    {
        unsigned short transaction_id, frame_number, frame_count;
        unsigned int data_length;
        unsigned char function_id;
    };
    struct SecondNetworkHeader
    {
        unsigned short transaction_id, frame_number, frame_count;
        unsigned int data_length;
        unsigned char function_id;
        std::array<float, 3> position;
        float radius;
    };

    ohf::tcp::Socket sock;
    unsigned short transaction_id;

    void wait_for_bytes(unsigned int amount);
public:
    struct GameObjectDescription
    {
        unsigned int object_id;
        unsigned char object_type;
        float x, y, z;
        float rotation;
        std::array<char, 16> model_name;
        std::array<float, 3> aabb;
        std::array<char, 47> reserved;
    };
    #pragma pack(pop)

    NetProtocol() = default;
    NetProtocol(const std::string &address, unsigned short port) noexcept
    {
        connect(address, port);
    }
    ~NetProtocol();

    void connect(const std::string &address, unsigned short port) noexcept;
    std::string get_welcome_message();
    std::vector<GameObjectDescription> get_demo_scene();
    std::vector<GameObjectDescription> get_nearby_objects(glm::vec3 position, float radius);

    unsigned short get_transaction_id() const;
};