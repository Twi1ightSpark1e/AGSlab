#pragma once

#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>
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
public:
    int get_mouse_state(int button);
    int get_mouse_state(char coordinate);
    void set_mouse_state(int button, int state);
    void set_mouse_state(char coordinate, int value);

    int get_arrow_state(int code);
    void set_arrow_state(int code, int value);

    static CameraController& get_instance();
};