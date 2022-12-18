#include "Window.h"

Mango::Window::Window(uint32_t width, uint32_t height)
{
    _width = width;
    _height = height;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _window = glfwCreateWindow(static_cast<int>(_width), static_cast<int>(_height), "Mango Engine", nullptr, nullptr);
}

Mango::Window::~Window()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}
