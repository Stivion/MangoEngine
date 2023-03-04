#include "GLFWWindow.h"

Mango::GLFWWindow::GLFWWindow(uint32_t width, uint32_t height) : Window(width, height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    _window = glfwCreateWindow(static_cast<int>(_width), static_cast<int>(_height), "Mango Engine", nullptr, nullptr);
    _framebufferResizedCallback = nullptr;
    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, GLFWKeyCallback);
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

void Mango::GLFWWindow::PollEvents()
{
    glfwPollEvents();
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

void Mango::GLFWWindow::GLFWKeyCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
    // Get action
    Mango::KeyAction keyAction = Mango::KeyAction::None;
    switch (action)
    {
    case GLFW_PRESS:
        keyAction = Mango::KeyAction::Press;
        break;
    case GLFW_RELEASE:
        keyAction = Mango::KeyAction::Release;
        break;
    case GLFW_REPEAT:
        keyAction = Mango::KeyAction::Repeat;
        break;
    }

    // Get key
    Mango::Key key = Mango::Key::None;
    switch (keycode)
    {
    case GLFW_KEY_W:
        key = Mango::Key::W;
        break;
    case GLFW_KEY_A:
        key = Mango::Key::A;
        break;
    case GLFW_KEY_S:
        key = Mango::Key::S;
        break;
    case GLFW_KEY_D:
        key = Mango::Key::D;
        break;
    case GLFW_KEY_SPACE:
        key = Mango::Key::SPACE;
        break;
    case GLFW_KEY_UP:
        key = Mango::Key::ARROW_UP;
        break;
    case GLFW_KEY_DOWN:
        key = Mango::Key::ARROW_DOWN;
        break;
    case GLFW_KEY_LEFT:
        key = Mango::Key::ARROW_LEFT;
        break;
    case GLFW_KEY_RIGHT:
        key = Mango::Key::ARROW_RIGHT;
        break;
    case GLFW_KEY_Q:
        key = Mango::Key::Q;
        break;
    case GLFW_KEY_E:
        key = Mango::Key::E;
        break;
    case GLFW_KEY_R:
        key = Mango::Key::R;
        break;
    }

    Mango::Input::KeyCallback(key, keyAction);
}

void Mango::GLFWWindow::GLFWWindowFramebufferResizedCallback(GLFWwindow* window, int width, int height)
{
    const auto& windowWrapper = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    windowWrapper->_framebufferResizedCallback(windowWrapper, width, height);
}
