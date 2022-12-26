#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class RenderPass
    {
    public:
        // TODO: Fix render pass creation
        RenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, bool hack);
        RenderPass() = delete;
        RenderPass(const RenderPass&) = delete;
        RenderPass operator=(const RenderPass&) = delete;
        ~RenderPass();

        void RecreateRenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain);

        const VkRenderPass& GetRenderPass() const { return _renderPass; }
        
    private:
        VkRenderPass _renderPass;
        VkDevice& _logicalDevice;
        bool _hack;

    private:
        void DisposeVulkanObjects();
        void CreateRenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, bool hack);
    };
}
