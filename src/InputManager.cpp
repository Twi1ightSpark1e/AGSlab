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
    return GLUT_UP;
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
    return GLUT_UP;
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

void InputManager::set_handlers(Camera &camera)
{
    this->camera = &camera;
    // функция, которая регистрирует перемещение мыши с зажатой кнопкой
    glutMotionFunc(InputManager::motion_func);
    // функция, которая вызывается каждый раз, когда нажимается кнопка мыши, или крутится колесо
    glutMouseFunc(InputManager::mouse_func);
    // функции обработки специальных кнопок
    glutSpecialFunc(InputManager::speckey_down_func);
    glutSpecialUpFunc(InputManager::speckey_up_func);

    glutKeyboardFunc(InputManager::keyboard_down_func);
    //glutKeyboardUpFunc(InputManager::keyboard_up_func);
}

void InputManager::set_key_handler(unsigned char key, InputManager::handler_func_t handler)
{
    events[key] = handler;
}

void InputManager::del_key_handler(unsigned char key)
{
    events.extract(key);
}

// Обработчики
void InputManager::motion_func(int x, int y)
{
    get_instance().set_mouse_state('x', x);
    get_instance().set_mouse_state('y', y);
}

void InputManager::mouse_func(int button, int state, int x, int y)
{
    switch (button)
    {
        case 0:
        case 1:
        case 2:
            get_instance().set_mouse_state(button, state);
            motion_func(x, y);
            return;
        case 3: // колёсико вверх
            get_instance().camera->zoom(0.05);
            return;
        case 4: // колёсико вниз
            get_instance().camera->zoom(-0.05);
            return;
        default:
            return;
    }
}

void InputManager::speckey_down_func(int key, int, int)
{
    speckey(key, GLUT_DOWN);
}

void InputManager::speckey_up_func(int key, int, int)
{
    speckey(key, GLUT_UP);
}

void InputManager::keyboard_down_func(unsigned char key, int, int)
{
    auto it = get_instance().events.find(key);
    if (it != get_instance().events.end())
    {
        it->second();
    }
}

// Вспомогательные для обработчиков методы
void InputManager::speckey(int key, int state)
{
    switch (key)
    {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            get_instance().set_arrow_state(key, state);
            return;
    }
}
