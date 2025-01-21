#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include <iostream>
#include "Game.h"
#include <glm/gtx/string_cast.hpp>
#include "EventManager.h"

Camera::Camera(float aspectRatio, float fov, glm::vec3 position, glm::vec3 up, glm::vec3 front)
    : fov_(fov), position_(position), up_(up), front_(front), m_left(glm::normalize(glm::cross(up_, front_)))
{
    yaw_ = glm::degrees(atan2(front_.z, front_.x));
    pitch_ = glm::degrees(atan2(front_.y, sqrt(front_.x * front_.x + front_.z * front_.z)));


    _updateViewMatrix();
    _updateProjectionMatrix(aspectRatio);

    EventHandler<WindowResizeEvent> windowResizeHandler = [this](const WindowResizeEvent& event) {this->_updateProjectionMatrix((float)event.width / (float)event.height);};
    EventManager::getInstance().sub(std::make_unique<EventHandlerWrapper<WindowResizeEvent>>(windowResizeHandler));

    EventHandler<WindowUpdateEvent> windowUpdateHandler = [this](const WindowUpdateEvent& event) {this->_update(event.window);};
    EventManager::getInstance().sub(std::make_unique<EventHandlerWrapper<WindowUpdateEvent>>(windowUpdateHandler));

    EventHandler<MouseMoveEvent> mouseMoveEventHandler = [this](const MouseMoveEvent& event) {this->_rotateCamera(event);};
    EventManager::getInstance().sub(std::make_unique<EventHandlerWrapper<MouseMoveEvent>>(mouseMoveEventHandler));
}


void Camera::_updateProjectionMatrix(const float& aspectRatio)
{
    projM_ = glm::perspective(fov_, aspectRatio, 0.1f, 100.0f);
}


void Camera::_updateViewMatrix()
{
    viewM_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::_rotateCamera(const MouseMoveEvent& event)
{
    if (!m_inRotation) return;

    glm::vec2 mousePos = event.mousePos;
    glm::vec2 deltaPos = mousePos - glm::vec2(event.width / 2, event.height / 2);
    float speed = 0.1f;
    yaw_ += speed * deltaPos.x / 2;
    pitch_ -= speed * deltaPos.y / 2;

    float maxPitch = 89.0f;
    pitch_ = glm::clamp(pitch_, -maxPitch, maxPitch);
    glm::vec3 front;

    front.x = (cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)));
    front.y = sin(glm::radians(pitch_));
    front.z = (sin(glm::radians(yaw_)) * cos(glm::radians(pitch_)));

    front_ = glm::normalize(front);
    glfwSetCursorPos(event.window, event.width / 2, event.height / 2);
}

/* This function is an event handler, it is triggered with WindowUpdateEvent.
   WindowUpdateEvent is triggered for every time window is updated. */
void Camera::_update(GLFWwindow* window)
{
    glm::vec3 dx = { 0.0f, 0.0f, 0.0f };
    m_left = glm::normalize(glm::cross(up_, front_));

    if (glfwGetKey(window, GLFW_KEY_W)) dx += front_;
    if (glfwGetKey(window, GLFW_KEY_S)) dx -= front_;
    if (glfwGetKey(window, GLFW_KEY_A)) dx += m_left;
    if (glfwGetKey(window, GLFW_KEY_D)) dx -= m_left;
    if (glfwGetKey(window, GLFW_KEY_SPACE)) dx += up_;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) dx -= up_;

    // escape for camera mode
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        m_inRotation = true;
    }

    // c for debug mode
    if (glfwGetKey(window, GLFW_KEY_C) && m_inRotation)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_inRotation = false;
    }

    if (dx != glm::vec3(0.0f, 0.0f, 0.0f)) position_ +=  0.01f * glm::normalize(dx);
    _updateViewMatrix();
}