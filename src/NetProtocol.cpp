#include <cgraphics/NetProtocol.hpp>

#include <libwire/memory_view.hpp>

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
            ioctl(sock.native_handle(), FIONREAD, &bytes_to_read);
        #endif
    }
}

void NetProtocol::connect(const std::string &address, unsigned short port) noexcept
{
    try
    {
        std::cout << "Trying connect to " << address << ":" << port << std::endl;
        sock.connect({ {address}, port });
        std::cout << "Connected successfully!" << std::endl;
        transaction_id = 0;
    }
    catch (std::system_error &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.what() << std::endl;
    }
}

std::string NetProtocol::get_welcome_message()
{
    if (!sock.is_open())
    {
        std::cout << "Cannot send welcome message - socket is closed" << std::endl;
        return {};
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
        libwire::memory_view request_buffer(&request, sizeof(request));
        sock.write(request_buffer);
        wait_for_bytes(request_buffer.size());
        sock.read(request_buffer.size(), request_buffer);
        if (request.data_length > 1)
        {
            wait_for_bytes(request.data_length - 1);
            auto buf = sock.read(request.data_length - 1);
            std::string str(reinterpret_cast<char*>(buf.data()), buf.size());
            return { str.substr(0, str.find('\0')) };
        }
        std::cout << "Cannot receive welcome message - it is too short!" << std::endl;
    }
    catch (std::system_error &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.what() << std::endl;
    }
    return {};
}

std::vector<NetProtocol::GameObjectDescription> NetProtocol::get_demo_scene()
{
    if (!sock.is_open())
    {
        std::cout << "Cannot get demo scene - socket is closed" << std::endl;
        return {};
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
        libwire::memory_view request_buffer(&request, sizeof(request));
        sock.write(request_buffer);
        wait_for_bytes(request_buffer.size());
        sock.read(request_buffer.size(), request_buffer);
        if (request.data_length > 1)
        {
            unsigned int count;
            libwire::memory_view count_buffer(&count, sizeof(count));
            wait_for_bytes(sizeof(int));
            sock.read(count_buffer.size(), count_buffer);
            GameObjectDescription descr;
            libwire::memory_view descr_buffer(&descr, sizeof(descr));
            for (unsigned int i = 0; i < count; i++)
            {
                wait_for_bytes(descr_buffer.size());
                sock.read(descr_buffer.size(), descr_buffer);
                out.push_back(descr);
            }
        }
        else
        {
            std::cout << "Cannot get demo scene - not enough information from server!" << std::endl;
        }
    }
    catch (std::system_error &exc)
    {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << exc.what() << std::endl;
    }
    return out;
}

void NetProtocol::nearby_objects_polling()
{
    using namespace std::chrono_literals;

    while (!stop_thread)
    {
        if (!sock.is_open())
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
            libwire::memory_view request_buffer(&request, sizeof(request));
            sock.write(request_buffer);
            wait_for_bytes(sizeof(NetworkHeader));
            sock.read(sizeof(NetworkHeader), request_buffer);
            if (request.data_length > 1)
            {
                unsigned int count;
                libwire::memory_view count_buffer(&count, sizeof(count));
                wait_for_bytes(count_buffer.size());
                sock.read(count_buffer.size(), count_buffer);
                GameObjectDescription descr;
                libwire::memory_view descr_buffer(&descr, sizeof(descr));
                for (unsigned int i = 0; i < count; i++)
                {
                    wait_for_bytes(descr_buffer.size());
                    sock.read(descr_buffer.size(), descr_buffer);
                    out.push_back(descr);
                }
            }
            else
            {
                std::cout << "Cannot get nearby objects - not enough information from server!" << std::endl;
                std::this_thread::sleep_for(500ms);
                continue;
            }
        }
        catch (std::system_error &exc)
        {
            std::cout << "Something went wrong!" << std::endl;
            std::cout << exc.what() << std::endl;
            std::this_thread::sleep_for(500ms);
            continue;
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
