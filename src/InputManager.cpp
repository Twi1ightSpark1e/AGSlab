#include <cgraphics/InputManager.hpp>

#include <algorithm>
#include <functional>

int InputManager::get_mouse_state(int field) const
{
    auto it = mouse.find(field);
    if (it != mouse.end())
    {
        return it->second;
    }
    return GLFW_RELEASE;
}

void InputManager::set_mouse_state(int field, int state)
{
    mouse[field] = state;
}

int InputManager::get_arrow_state(int code) const
{
    auto it = arrows.find(code);
    if (it != arrows.end())
    {
        return it->second;
    }
    return GLFW_RELEASE;
}

void InputManager::set_arrow_state(int code, int value)
{
    arrows[code] = value;
}

InputManager& InputManager::get_instance() noexcept
{
    static InputManager instance;
    return instance;
}

void InputManager::set_handlers(GLFWwindow *window, Camera &camera)
{
    this->camera = &camera;
    glfwSetCursorPosCallback(window, InputManager::motion_func);
    glfwSetMouseButtonCallback(window, InputManager::mouse_func);
    glfwSetScrollCallback(window, InputManager::scroll_func);
    glfwSetKeyCallback(window, InputManager::key_func);
}

void InputManager::set_key_handler(int key, InputManager::handler_func_t handler)
{
    events[key] = handler;
}

void InputManager::del_key_handler(int key)
{
    events.extract(key);
}

// Обработчики
void InputManager::motion_func(GLFWwindow*, double x, double y)
{
    get_instance().set_mouse_state('x', x);
    get_instance().set_mouse_state('y', y);
}

void InputManager::mouse_func(GLFWwindow*, int button, int action, int mods)
{
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        case GLFW_MOUSE_BUTTON_MIDDLE:
        case GLFW_MOUSE_BUTTON_RIGHT:
            get_instance().set_mouse_state(button, action);
            return;
    }
}

void InputManager::scroll_func(GLFWwindow*, double xoffset, double yoffset) {
    get_instance().camera->zoom(yoffset / 10);
}

void InputManager::key_func(GLFWwindow*, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_RELEASE) {
        return;
    }

    switch (key) {
        case GLFW_KEY_UP:
        case GLFW_KEY_DOWN:
        case GLFW_KEY_LEFT:
        case GLFW_KEY_RIGHT:
            get_instance().set_arrow_state(key, action);
            return;
        default:
            if (action == GLFW_PRESS) {
                auto it = get_instance().events.find(key);
                if (it != get_instance().events.end())
                {
                    it->second();
                }
            }
    }
}

