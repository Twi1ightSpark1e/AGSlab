#pragma once

#include <glm/glm.hpp>

class Camera
{
private:
    // Матрицы проекции и наблюдения
    glm::mat4 projection, view;
    // Вектора: местоположение глаз, точки обзора и направления вверх
    glm::vec3 eye, center, up;
    // Вспомогательные поля
    double radian_x = .6, radian_y = .4, radius = 28, speed = 3.5;
    double radius_min, radius_max;
    double oxz_min, oxz_max;

    void calculate_vectors();
public:
    Camera() noexcept;
    // clang-tidy, shut up!
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;

    ~Camera();

    void set_projection_matrix(float fovy, float aspect, float z_near, float z_far);
    const glm::mat4 &get_projection_matrix() const;

    const glm::mat4 &get_view_matrix() const;

    const glm::vec3 &get_eye() const;
    const glm::vec3 &get_center() const;

    void move_oxz(double forward, double right);
    void rotate(double horizontal, double vertical);
    void zoom(double radius);

    void set_radius(double current, double min, double max);
    void set_vertical(double current, double min, double max);
    void set_horizontal(double current);

    bool operator==(const Camera& b) const;
    bool operator!=(const Camera& b) const;
};
