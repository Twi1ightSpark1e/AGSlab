#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <fstream>

class Camera
{
private:
    // Матрицы проекции и наблюдения
    glm::mat4 projection, view;
    // Вектора: местоположение глаз, точки обзора и направления вверх
    glm::vec3 eye, center, up;
    // Вспомогательные поля
    double radian_x, radian_y, radius, speed;

    void calculate_vectors();
public:
    Camera();
    ~Camera();
    static Camera &get_instance();

    void set_projection_matrix(float fovy, float aspect, float z_near, float z_far);
    glm::mat4 get_projection_matrix();

    glm::mat4 get_view_matrix();

    void move_oxz(double forward, double right);
    void rotate(double horizontal, double vertical);
    void zoom(double radius);
};