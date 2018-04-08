#pragma once

#include <cgraphics/Camera.hpp>

#include <GL/freeglut.h>

#include <map>

class InputManager
{
private:
    std::map<int, int> mouse = {
        {GLUT_LEFT_BUTTON,   GLUT_UP},
        {GLUT_MIDDLE_BUTTON, GLUT_UP},
        {GLUT_RIGHT_BUTTON,  GLUT_UP}
    };
    std::map<int, int> arrows = {
        {GLUT_KEY_LEFT,      GLUT_UP},
        {GLUT_KEY_UP,        GLUT_UP},
        {GLUT_KEY_RIGHT,     GLUT_UP},
        {GLUT_KEY_DOWN,      GLUT_UP}
    };
    //Scene scene;
    Camera *camera;

    InputManager() = default;
    ~InputManager() = default;

    static void motion_func(int x, int y);
    static void mouse_func(int button, int state, int x, int y);
    static void speckey_down_func(int key, int x, int y);
    static void speckey_up_func(int key, int x, int y);
    // Вспомогательный метод
    static void speckey(int key, int state);
public:
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    int get_mouse_state(int field) const;
    void set_mouse_state(int field, int state);

    int get_arrow_state(int code) const;
    void set_arrow_state(int code, int value);

    void set_handlers(Camera &camera);

    static InputManager& get_instance() noexcept;
};
