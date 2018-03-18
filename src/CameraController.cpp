#include <cgraphics/CameraController.hpp>

#include <algorithm>

int CameraController::get_mouse_state(int button) const
{
    auto it = mouse.find(button);
    if (it != mouse.end())
    {
        return it->second;
    }
    return GLUT_UP;
}

int CameraController::get_mouse_state(char coordinate) const
{
    auto it = mouse.find((int)coordinate);
    if (it != mouse.end())
    {
        return it->second;
    }
    return GLUT_UP;
}

void CameraController::set_mouse_state(int button, int state)
{
    mouse[button] = state;
}

void CameraController::set_mouse_state(char coordinate, int value)
{
    mouse[(int)coordinate] = value;
}

int CameraController::get_arrow_state(int code) const
{
    auto it = arrows.find(code);
    if (it != arrows.end())
    {
        return it->second;
    }
    return GLUT_UP;
}

void CameraController::set_arrow_state(int code, int value)
{
    arrows[code] = value;
}

CameraController& CameraController::get_instance()
{
    static CameraController instance;
    return instance;
}