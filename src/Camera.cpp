#include <cgraphics/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <fstream>
#include <iostream>

Camera::Camera() noexcept : 
    center(0, 0, 0),
    up(0, 1, 0)
{
    #ifdef DEBUG
    std::ifstream dump("camera_position.txt");
    if (dump.is_open())
    {
        dump >> radian_x >> radian_y >> radius;
        dump >> center.x >> center.y >> center.z;
        dump.close();
    }
    #endif
    calculate_vectors();
}

Camera::~Camera()
{
    #ifdef DEBUG
    std::ofstream dump("camera_position.txt");
    dump << radian_x << ' ';
    dump << radian_y << ' ';
    dump << radius << ' ';

    dump << center.x << ' ';
    dump << center.y << ' ';
    dump << center.z << ' ';
    dump.flush();
    dump.close();
    #endif
}

void Camera::calculate_vectors()
{
    auto distance = radius * std::cos(radian_y);
    eye.x = distance * std::cos(radian_x);
    eye.y =   radius * std::sin(radian_y);
    eye.z = distance * std::sin(radian_x);

    view = glm::lookAt(eye + center, center, up);
}

void Camera::set_projection_matrix(float fovy, float aspect, float z_near, float z_far)
{
    projection = glm::perspective(fovy, aspect, z_near, z_far);
}

const glm::mat4 &Camera::get_projection_matrix() const
{
    return projection;
}

const glm::mat4 &Camera::get_view_matrix() const
{
    return view;
}

const glm::vec3 &Camera::get_eye() const
{
    return eye;
}

const glm::vec3 &Camera::get_center() const
{
    return center;
}

void Camera::move_oxz(double forward, double right)
{
    auto vec_forward = glm::normalize(-eye);
    auto delta_forward = glm::vec3(vec_forward.x * forward * speed, 0, vec_forward.z * forward * speed); 
    auto delta_right = glm::normalize(glm::cross(vec_forward, up));
    delta_right = glm::vec3(delta_right.x * right * speed, 0, delta_right.z * right * speed);

    center += delta_forward * float(speed) + delta_right * float(speed);

    calculate_vectors();
}

void Camera::rotate(double horizontal, double vertical)
{
    radian_x += horizontal * std::sqrt(speed);
    if ((radian_y >= oxz_min) && (radian_y <= oxz_max))
    {
        radian_y += vertical * std::sqrt(speed);
        radian_y = std::max(oxz_min, std::min(oxz_max, radian_y));
    }
    calculate_vectors();
}

void Camera::zoom(double radius)
{
    if ((this->radius >= radius_min) && (this->radius <= radius_max))
    {
        this->radius -= radius * std::pow(speed, 2);
        this->radius = std::max(radius_min, std::min(this->radius, radius_max));
    }
    calculate_vectors();
}

bool Camera::operator==(const Camera &b) const
{
    return ((view == b.view) && (projection == b.projection));
}

bool Camera::operator!=(const Camera &b) const
{
    return ((view != b.view) || (projection != b.projection));
}

void Camera::set_radius(double current, double min, double max)
{
    radius = current;
    radius_min = min;
    radius_max = max;

    calculate_vectors();
}

void Camera::set_vertical(double current, double min, double max)
{
    radian_y = glm::radians(current);
    oxz_min = glm::radians(min);
    oxz_max = glm::radians(max);

    calculate_vectors();
}

void Camera::set_horizontal(double current)
{
    radian_x = glm::radians(current);

    calculate_vectors();
}
