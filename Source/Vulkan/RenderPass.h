#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderPass
    {
    public:
        RenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain);
        RenderPass() = delete;
        RenderPass(const RenderPass&) = delete;
        RenderPass operator=(const RenderPass&) = delete;
        ~RenderPass();

        const VkRenderPass& GetRenderPass() const { return _renderPass; }
        
    private:
        VkRenderPass _renderPass;
        VkDevice& _logicalDevice;
    };
}
