#include "GLFWWindow.h"

Mango::GLFWWindow::GLFWWindow(uint32_t width, uint32_t height) : Window(width, height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    _window = glfwCreateWindow(static_cast<int>(_width), static_cast<int>(_height), "Mango Engine", nullptr, nullptr);
    _framebufferResizedCallback = nullptr;
    glfwSetWindowUserPointer(_window, this);
}

Mango::GLFWWindow::~GLFWWindow()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}

bool Mango::GLFWWindow::ShouldClose()
{
    return glfwWindowShouldClose(_window);
}

void Mango::GLFWWindow::SetFramebufferResizedCallback(FramebufferResizedCallback callback)
{
    _framebufferResizedCallback = callback;
    glfwSetFramebufferSizeCallback(_window, GLFWWindowFramebufferResizedCallback);
}

const uint32_t Mango::GLFWWindow::GetWindowWidth() const
{
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    return static_cast<uint32_t>(width);
}

const uint32_t Mango::GLFWWindow::GetWindowHeight() const
{
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    return static_cast<uint32_t>(height);
}

void Mango::GLFWWindow::GLFWWindowFramebufferResizedCallback(GLFWwindow* window, int width, int height)
{
    const auto& windowWrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    windowWrapper->_framebufferResizedCallback(windowWrapper, width, height);
}
