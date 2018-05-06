#include <cgraphics/NetProtocol.hpp>

#include <ohf/Exception.hpp>

#include <cstring>
#include <iostream>

#ifdef _WIN32
#   include <winsock2.h>
#else
#   include <sys/ioctl.h>
#endif

NetProtocol::~NetProtocol()
{
    using namespace std::chrono_literals;

    stop_thread = true;
    if (polling.joinable())
    {
        polling.join();
    }
}

void NetProtocol::wait_for_bytes(unsigned int amount)
{
    unsigned long bytes_to_read = 0;
    while (bytes_to_read < amount)
    {
        #ifdef _WIN32
            ioctlsocket(sock.fd(), FIONREAD, &bytes_to_read);
        #else
            ioctl(sock.fd(), FIONREAD, &bytes_to_read);
        #endif
    }
}

void NetProtocol::connect(const std::string &address, unsigned short port) noexcept
{
    try
    {
        std::cout << "Trying connect to " << address << ":" << port << std::endl;
        sock.connect(ohf::InetAddress(address), port);
        std::cout << "Connected successfully!" << std::endl;
        transaction_id = 0;
    }
    catch (ohf::Exception &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.message() << std::endl;
    }
}

std::string NetProtocol::get_welcome_message()
{
    if (!sock.isValid())
    {
        std::cout << "Cannot send welcome message - socket is closed" << std::endl;
        return std::string();
    }
    NetworkHeader request = {
        transaction_id++, // transaction_id
        1, // frame_number
        1, // frame_count
        1, // data_length
        0  // function_id
    };
    try
    {
        sock.send(reinterpret_cast<char*>(&request), sizeof(request));
        wait_for_bytes(sizeof(request));
        sock.receive(reinterpret_cast<char*>(&request), sizeof(request));
        if (request.data_length > 1)
        {
            wait_for_bytes(request.data_length - 1);
            auto str = sock.receiveString(request.data_length - 1);
            str = str.substr(0, str.find('\0'));
            return str;
        }
        std::cout << "Cannot send welcome message - it is too short!" << std::endl;
    }
    catch (ohf::Exception &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.message() << std::endl;
    }
    return std::string();
}

std::vector<NetProtocol::GameObjectDescription> NetProtocol::get_demo_scene()
{
    if (!sock.isValid())
    {
        std::cout << "Cannot get demo scene - socket is closed" << std::endl;
        
    }
    NetworkHeader request = {
        transaction_id++, // transaction_id
        1, // frame_number
        1, // frame_count
        1, // data_length
        1  // function_id
    };
    std::vector<GameObjectDescription> out;
    try
    {
        sock.send(reinterpret_cast<char*>(&request), sizeof(NetworkHeader));
        wait_for_bytes(sizeof(NetworkHeader));
        sock.receive(reinterpret_cast<char*>(&request), sizeof(NetworkHeader));
        if (request.data_length > 1)
        {
            unsigned int count;
            wait_for_bytes(sizeof(int));
            sock.receive(reinterpret_cast<char*>(&count), sizeof(int));
            GameObjectDescription descr;
            for (unsigned int i = 0; i < count; i++)
            {
                wait_for_bytes(sizeof(GameObjectDescription));
                sock.receive(reinterpret_cast<char*>(&descr), sizeof(GameObjectDescription));
                out.push_back(descr);
            }
        }
        else
        {
            std::cout << "Cannot get demo scene - not enough information from server!" << std::endl;
        }
    }
    catch (ohf::Exception &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.message() << std::endl;
    }
    return out;
}

void NetProtocol::nearby_objects_polling()
{
    using namespace std::chrono_literals;

    while (!stop_thread)
    {
        if (!sock.isValid())
        {
            std::cout << "Cannot get nearby objects - socket is closed" << std::endl;
            std::this_thread::sleep_for(500ms);
            continue;
        }

        parameters_synchronizer.lock();
        SecondNetworkHeader request = {
            transaction_id++, // transaction_id
            1,                // frame_number
            1,                // frame_count
            17,               // data_length
            2,                // function_id
            {
                last_position.x,       // eye_position_x
                last_position.y,       // eye_position_y
                last_position.z,       // eye_position_z
            },
            last_radius            // radius
        };
        parameters_synchronizer.unlock();

        std::vector<GameObjectDescription> out;
        try
        {
            sock.send(reinterpret_cast<char*>(&request), sizeof(SecondNetworkHeader));
            wait_for_bytes(sizeof(NetworkHeader));
            sock.receive(reinterpret_cast<char*>(&request), sizeof(NetworkHeader));
            if (request.data_length > 1)
            {
                unsigned int count;
                wait_for_bytes(sizeof(int));
                sock.receive(reinterpret_cast<char*>(&count), sizeof(int));
                GameObjectDescription descr;
                for (unsigned int i = 0; i < count; i++)
                {
                    wait_for_bytes(sizeof(GameObjectDescription));
                    sock.receive(reinterpret_cast<char*>(&descr), sizeof(GameObjectDescription));
                    out.push_back(descr);
                }
            }
            else
            {
                std::cout << "Cannot get nearby objects - not enough information from server!" << std::endl;
            }
        }
        catch (ohf::Exception &exc)
        {
            std::cout << "Something went wrong!" << std::endl;
            std::cout << exc.message() << std::endl;
        }

        result_synchronizer.lock();
        nearby_objects = std::move(out);
        result_synchronizer.unlock();
    }
}

std::vector<NetProtocol::GameObjectDescription> NetProtocol::get_nearby_objects(glm::vec3 position, float radius)
{
    parameters_synchronizer.lock();
    last_position = position;
    last_radius = radius;
    parameters_synchronizer.unlock();

    if (!polling.joinable())
    {
        stop_thread = false;
        polling = std::thread(&NetProtocol::nearby_objects_polling, this);
    }

    std::lock_guard lock(result_synchronizer);
    return nearby_objects;
}
