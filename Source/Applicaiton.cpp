#include "Application.h"

void Mango::Application::Run()
{
    InitializeWindow(800, 600);
    InitializeVulkan();
    RunMainLoop();
    Dispose();
}

void Mango::Application::InitializeWindow(uint32_t width, uint32_t height)
{
    _windowWidth = width;
    _windowHeight = height;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _window = glfwCreateWindow(static_cast<int>(_windowWidth), static_cast<int>(_windowHeight), "Learning Vulkan", nullptr, nullptr);
    
}

void Mango::Application::InitializeVulkan()
{
    _instance.CreateInstance();
    _renderSurface.CreateRenderSurface(_window, &_instance);
    _physicalDevice.PickPhysicalDevice(_instance, _renderSurface);
    _logicalDevice.CreateLogicalDevice(_physicalDevice, _renderSurface);
    _swapChain.CreateSwapChain(_window, _physicalDevice, _logicalDevice, _renderSurface);
    _graphicsPipeline.CreateGraphicsPipeline(_logicalDevice, _swapChain);
}

void Mango::Application::RunMainLoop()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();
    }
}

void Mango::Application::Dispose()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}
