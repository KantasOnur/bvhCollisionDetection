#pragma once
#include <glm/glm.hpp>
#include "Window.h"
#include "Events.h"

class Camera
{
private:
    glm::vec3 position_, front_, up_, right_, m_left;
    glm::mat4 projM_, viewM_;
    float pitch_, yaw_, fov_;
    bool m_inRotation = false;
public:
    explicit Camera(float aspectRatio, float fov = 45.0f, glm::vec3 position = { 0.0f, 0.0f, 10.0f }, glm::vec3 up = { 0.0f, 1.0f, 0.0f }, glm::vec3 front = { 0.0f, 0.0f, -1.0f });
    ~Camera() = default;

    glm::mat4 getProjection() const { return projM_; }
    glm::mat4 getView() const { return viewM_; }
    glm::vec3 getPosition() const { return position_; }

private:
    void _updateProjectionMatrix(const float& aspectRatio);
    void _update(GLFWwindow* window);
    void _updateViewMatrix();
    void _rotateCamera(const MouseMoveEvent& event);
};