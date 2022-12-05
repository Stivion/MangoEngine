#pragma once

#include "Instance.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderSurface
    {
    public:
        RenderSurface() = default;
        RenderSurface(const RenderSurface&) = delete;
        RenderSurface operator=(const RenderSurface&) = delete;
        ~RenderSurface();
        
        void CreateRenderSurface(GLFWwindow* window, Mango::Instance* instance);
        
        VkSurfaceKHR& GetRenderSurface() { return _renderSurface; }
    private:
        Mango::Instance* _instance;
        VkSurfaceKHR _renderSurface;
    };
}
