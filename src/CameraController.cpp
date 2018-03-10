#include <cgraphics/CameraController.hpp>

int CameraController::get_mouse_state(int button)
{
    return mouse[button];
}

int CameraController::get_mouse_state(char coordinate)
{
    return mouse[(int)coordinate];
}

void CameraController::set_mouse_state(int button, int state)
{
    mouse[button] = state;
}

void CameraController::set_mouse_state(char coordinate, int value)
{
    mouse[(int)coordinate] = value;
}

int CameraController::get_arrow_state(int code)
{
    return arrows[code];
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