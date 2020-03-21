#include <cgraphics/NetProtocol.hpp>

#include <cstring>
#include <iostream>

#ifdef _WIN32
#   include <winsock2.h>
#else
#   include <sys/ioctl.h>
#endif

using namespace boost::asio;

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
    std::cout << "Trying connect to " << address << ":" << port << std::endl;

    boost::system::error_code ec;
    sock.connect(ip::tcp::endpoint(ip::address::from_string(address), port), ec);
    if (ec) {
        std::cout << "Something went wrong!" << std::endl;
        std::cout << ec.message() << std::endl;
        return;
    }

    std::cout << "Connected successfully!" << std::endl;
    transaction_id = 0;
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
    auto buf = boost::asio::buffer(&request, sizeof(request));

    try
    {
        sock.write_some(buf);
        wait_for_bytes(buf.size());
        sock.read_some(buf);

        if (request.data_length < 1) {
            std::cout << "Cannot receive welcome message - it is too short!" << std::endl;
            return {};
        }

        char *message = new char[request.data_length - 1];
        auto received = sock.receive(buffer(message, request.data_length - 1));
        std::string str(message, received);
        delete[] message;
        return { str.substr(0, str.find('\0')) };
    }
    catch (const std::exception &exc)
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
    auto request_buf = boost::asio::buffer(&request, sizeof(request));
    std::vector<GameObjectDescription> out;
    try
    {
        sock.write_some(request_buf);
        wait_for_bytes(request_buf.size());
        sock.read_some(request_buf);
        if (request.data_length < 1) {
            std::cout << "Cannot get demo scene - not enough information from server!" << std::endl;
            return out;
        }

        unsigned int count;
        auto count_buf = boost::asio::buffer(&count, sizeof(count));
        wait_for_bytes(count_buf.size());
        sock.read_some(count_buf);
        out.reserve(count);

        GameObjectDescription descr;
        auto descr_buf = boost::asio::buffer(&descr, sizeof(descr));

        for (auto i = 0; i < count; i++) {
            wait_for_bytes(descr_buf.size());
            sock.read_some(descr_buf);
            out.push_back(std::move(descr));
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
        auto request_buf = boost::asio::buffer(&request, sizeof(request));
        NetworkHeader answer;
        auto answer_buf = boost::asio::buffer(&answer, sizeof(answer));

        std::vector<GameObjectDescription> out;
        try
        {
            sock.write_some(request_buf);
            wait_for_bytes(answer_buf.size());
            sock.receive(answer_buf);
            if (answer.data_length < 1) {
                std::cout << "Cannot get nearby objects - not enough information from server!" << std::endl;
                std::this_thread::sleep_for(500ms);
                continue;
            }


            std::uint32_t count;
            auto count_buf = boost::asio::buffer(&count, sizeof(count));
            wait_for_bytes(count_buf.size());
            sock.receive(count_buf);
            out.reserve(count);

            GameObjectDescription descr;
            auto descr_buf = boost::asio::buffer(&descr, sizeof(descr));

            for (auto i = 0; i < count; i++) {
                wait_for_bytes(descr_buf.size());
                sock.receive(descr_buf);
                out.push_back(std::move(descr));
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
