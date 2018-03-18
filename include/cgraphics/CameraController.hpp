#pragma once

#include <GL/freeglut.h>

#include <map>

class CameraController
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

    CameraController() {};
    ~CameraController() {};
    CameraController(const CameraController&) = delete;
    CameraController(CameraController&&) = delete;
    CameraController& operator=(const CameraController&) = delete;
    CameraController& operator=(CameraController&&) = delete;
public:
    int get_mouse_state(int button) const;
    int get_mouse_state(char coordinate) const;
    void set_mouse_state(int button, int state);
    void set_mouse_state(char coordinate, int value);

    int get_arrow_state(int code) const;
    void set_arrow_state(int code, int value);

    static CameraController& get_instance();
};