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
    double radian_x = .2, radian_y = .2, radius = 14, speed = 2;

    void calculate_vectors();
public:
    Camera() noexcept;
    // clang-tidy, shut up!
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;

    ~Camera();
    //static Camera &get_instance();

    void set_projection_matrix(float fovy, float aspect, float z_near, float z_far);
    const glm::mat4 &get_projection_matrix() const;

    const glm::mat4 &get_view_matrix() const;

    void move_oxz(double forward, double right);
    void rotate(double horizontal, double vertical);
    void zoom(double radius);
};
