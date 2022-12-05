#pragma once

#define GLFW_INCLUDE_VULKAN

#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/RenderSurface.h"

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
        Mango::Instance _vkInstance;
        Mango::RenderSurface _vkRenderSurface;
        Mango::PhysicalDevice _vkPhysicalDevice;
        Mango::LogicalDevice _vkLogicalDevice;
    };

}
