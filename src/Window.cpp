#include "Window.h"
#include "Game.h"
#include "EventManager.h"

#include <iostream>

int Window::width_;
int Window::height_;

Window::Window(const int& width, const int& height)
{
    width_ = width;
    height_ = height;

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width_, height_, "Game", nullptr, nullptr);
    glfwMakeContextCurrent(window_);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW." << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwSetWindowSizeCallback(window_, windowResizeCallback);
    glfwSetCursorPosCallback(window_, mouseMoveCallback);

    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);

    std::cout << "GPU Vendor: " << vendor << std::endl;
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL Version: " << version << std::endl;

    glfwSwapInterval(0); // set vsync

}

Window::~Window()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::update() const
{
    /*
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("yoyo");
    ImGui::Text("bap");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    */

    glfwSwapBuffers(window_);
    glfwPollEvents();


    WindowUpdateEvent e(window_);
    EventManager::getInstance().triggerEvent(e);
}

double Window::getTime()
{
    return glfwGetTime();
}

float Window::getDeltaTime()
{
    float current = getTime();
    float dt = current - lastTime_;
    lastTime_ = current;
    return dt;
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(window_);
}


void Window::windowResizeCallback(GLFWwindow* window, int width, int height)
{
    width_ = width;
    height_ = height;
    glViewport(0, 0, width, height);
    WindowResizeEvent e(width, height);
    EventManager::getInstance().triggerEvent(e);
}

void Window::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    MouseMoveEvent e(xpos, ypos, width_, height_, window);
    EventManager::getInstance().triggerEvent(e);
}