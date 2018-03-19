#include <cgraphics/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <fstream>

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

/*Camera &Camera::get_instance()
{
    static Camera instance;
    return instance;
}*/

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

void Camera::move_oxz(double forward, double right)
{
    auto vec_forward = glm::normalize(-eye);
    auto delta_forward = glm::vec3(vec_forward.x * forward, 0, vec_forward.z * forward); 
    auto delta_right = glm::normalize(glm::cross(vec_forward, up));
    delta_right = glm::vec3(delta_right.x * right, 0, delta_right.z * right);

    center += delta_forward * float(speed) + delta_right * float(speed);

    calculate_vectors();
}

void Camera::rotate(double horizontal, double vertical)
{
    static const double LOWER_BOUND = .0873, UPPER_BOUND = 1.394;

    radian_x += horizontal * std::sqrt(speed);
    if ((radian_y >= LOWER_BOUND) && (radian_y <= UPPER_BOUND))
    {
        radian_y += vertical * std::sqrt(speed);
        radian_y = std::max(LOWER_BOUND, std::min(UPPER_BOUND, radian_y));
    }
    calculate_vectors();
}

void Camera::zoom(double radius)
{
    static const double LOWER_BOUND = 2, UPPER_BOUND = 100;

    if ((this->radius >= LOWER_BOUND) && (this->radius <= UPPER_BOUND))
    {
        this->radius -= radius * std::pow(speed, 2);
        this->radius = std::max(LOWER_BOUND, std::min(this->radius, UPPER_BOUND));
    }
    calculate_vectors();
}
