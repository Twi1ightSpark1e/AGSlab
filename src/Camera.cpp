#include <cgraphics/Camera.hpp>

Camera::Camera() : 
    center(0, 0, 0),
    up(0, 1, 0),
    radian_x(0),
    radian_y(0.2),
    radius(10.f),
    speed(2)
{
    std::ifstream dump("camera_position.txt");
    if (dump.is_open())
    {
        dump >> radian_x >> radian_y >> radius;
        dump >> center.x >> center.y >> center.z;
        dump.close();
    }
    calculate_vectors();
}

Camera::~Camera()
{
    std::ofstream dump("camera_position.txt");
    dump << radian_x << ' ';
    dump << radian_y << ' ';
    dump << radius << ' ';

    dump << center.x << ' ';
    dump << center.y << ' ';
    dump << center.z << ' ';
    dump.flush();
    dump.close();
}

Camera &Camera::get_instance()
{
    static Camera instance;
    return instance;
}

void Camera::calculate_vectors()
{
    auto distance = radius * std::cos(radian_y);
    eye.x = distance * std::cos(radian_x);
    eye.y = radius * std::sin(radian_y);
    eye.z = distance * std::sin(radian_x);

    view = glm::lookAt(eye + center, center, up);
}

void Camera::set_projection_matrix(float fovy, float aspect, float z_near, float z_far)
{
    projection = glm::perspective(fovy, aspect, z_near, z_far);
}

glm::mat4 Camera::get_projection_matrix()
{
    return projection;
}

glm::mat4 Camera::get_view_matrix()
{
    return view;
}

void Camera::move_oxz(double forward, double right)
{
    center.x -= forward * speed;
    center.z -= right * speed;
    calculate_vectors();
}

void Camera::rotate(double horizontal, double vertical)
{
    radian_x += horizontal * std::sqrt(speed);
    if ((radian_y > 0.0873) && (radian_y < 1.396))
    {
        radian_y += vertical * std::sqrt(speed);
        radian_y = std::max(.0873, std::min(1.396, radian_y));
    }
    calculate_vectors();
}

void Camera::zoom(double radius)
{
    if ((this->radius > 2) && (this->radius < 100))
    {
        this->radius -= radius * std::pow(speed, 2);
    }
    calculate_vectors();
}