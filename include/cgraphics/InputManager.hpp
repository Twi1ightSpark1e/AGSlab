#pragma once

#include <cgraphics/Camera.hpp>

#include <GLFW/glfw3.h>

#include <functional>
#include <map>

class InputManager
{
private:
    typedef std::function<void()> handler_func_t;
    std::map<int, int> mouse = {
        { GLFW_MOUSE_BUTTON_LEFT,   GLFW_RELEASE },
        { GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE },
        { GLFW_MOUSE_BUTTON_RIGHT,  GLFW_RELEASE }
    };
    std::map<int, int> arrows = {
        { GLFW_KEY_LEFT,      GLFW_RELEASE },
        { GLFW_KEY_UP,        GLFW_RELEASE },
        { GLFW_KEY_RIGHT,     GLFW_RELEASE },
        { GLFW_KEY_DOWN,      GLFW_RELEASE }
    };
    std::map<int, handler_func_t> events;
    Camera *camera;

    InputManager() = default;
    ~InputManager() = default;

    static void motion_func(GLFWwindow*, double, double);
    static void mouse_func(GLFWwindow*, int, int, int);
    static void scroll_func(GLFWwindow*, double, double);
    static void key_func(GLFWwindow*, int, int, int, int);
public:
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    int get_mouse_state(int field) const;
    void set_mouse_state(int field, int state);

    int get_arrow_state(int code) const;
    void set_arrow_state(int code, int value);

    void set_key_handler(int key, handler_func_t handler);
    void del_key_handler(int key);

    void set_handlers(GLFWwindow*, Camera&);

    static InputManager& get_instance() noexcept;
};
