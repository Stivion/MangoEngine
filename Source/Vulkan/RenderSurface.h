#pragma once

#include "Instance.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderSurface
    {
    public:
        RenderSurface(const Mango::Instance& instance);
        RenderSurface() = delete;
        RenderSurface(const RenderSurface&) = delete;
        RenderSurface operator=(const RenderSurface&) = delete;
        ~RenderSurface();
        
        const VkSurfaceKHR& GetRenderSurface() const { return _renderSurface; }

    protected:
        VkSurfaceKHR _renderSurface;
        const VkInstance& _instance;
    };
}
