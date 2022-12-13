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
    _imageView.CreateImageViews(_logicalDevice, _swapChain);
    _renderPass.CreateRenderPass(_logicalDevice, _swapChain);
    _graphicsPipeline.CreateGraphicsPipeline(_logicalDevice, _swapChain, _renderPass);
    _framebuffer.CreateFramebuffers(_logicalDevice, _swapChain, _imageView, _renderPass);
    _commandPool.CreateCommandPool(_physicalDevice, _logicalDevice, _renderSurface);
    _commandBuffer.CreateCommandBuffer(_logicalDevice, _swapChain, _renderPass, _graphicsPipeline, _framebuffer, _commandPool);
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
