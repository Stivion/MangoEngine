#pragma once

#include "../Windowing/Window.h"
#include "Instance.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderSurface
    {
    public:
        RenderSurface(Mango::Window& window, Mango::Instance& instance);
        RenderSurface() = delete;
        RenderSurface(const RenderSurface&) = delete;
        RenderSurface operator=(const RenderSurface&) = delete;
        ~RenderSurface();
        
        VkSurfaceKHR& GetRenderSurface() { return _renderSurface; }
    private:
        VkSurfaceKHR _renderSurface;

        VkInstance& _instance;
        GLFWwindow* _window;
    };
}
