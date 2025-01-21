#pragma once
#include "OpenGL.h"
struct params
{
    int width;
    int height;
};
class Window
{
private:
    GLFWwindow* window_;
    static int width_, height_;
    float lastTime_ = 0.0f;
    void static windowResizeCallback(GLFWwindow* window, int width, int height);
    void static mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
public:
    Window(const int& width, const int& height);
    ~Window();
    void update() const;
    static double getTime();
    float getDeltaTime();
    bool isOpen() const;
    float getAspectRatio() const { return (float)width_ / (float)height_; }
    params getParams() const { return { width_, height_ }; }
    GLFWwindow* getWindow() const { return window_; }
};