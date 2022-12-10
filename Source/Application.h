#pragma once

#define GLFW_INCLUDE_VULKAN

#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/RenderSurface.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/GraphicsPipeline.h"

#include <GLFW/glfw3.h>
#include <cstdint>

namespace Mango
{

    class Application
    {
    public:
        Application() = default;
        Application(const Application&) = delete;
        Application operator=(const Application&) = delete;
        
        void Run();

    private:
        void InitializeWindow(uint32_t width, uint32_t height);
        void InitializeVulkan();
        void RunMainLoop();
        void Dispose();
        
    private:
        uint32_t _windowWidth;
        uint32_t _windowHeight;
        GLFWwindow* _window;
        Mango::Instance _instance;
        Mango::RenderSurface _renderSurface;
        Mango::PhysicalDevice _physicalDevice;
        Mango::LogicalDevice _logicalDevice;
        Mango::SwapChain _swapChain;
        Mango::ImageView _imageView;
        Mango::GraphicsPipeline _graphicsPipeline;
    };

}
