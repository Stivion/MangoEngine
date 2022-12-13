#pragma once

#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderPass
    {
    public:
        RenderPass() = default;
        RenderPass(const RenderPass&) = delete;
        RenderPass operator=(const RenderPass&) = delete;
        ~RenderPass();
        
        void CreateRenderPass(LogicalDevice& logicalDevice, SwapChain& swapChain);
        VkRenderPass& GetRenderPass() { return _renderPass; }
    private:
        LogicalDevice* _logicalDevice;
        SwapChain* _swapChain;
        VkRenderPass _renderPass;
    };
}
