#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    struct RenderPassCreateInfo
    {
        VkImageLayout ColorAttachmentReferenceLayout;
        VkImageLayout ColorAttachmentFinalLayout;
    };

    class RenderPass
    {
    public:
        RenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, const RenderPassCreateInfo& createInfo);
        RenderPass() = delete;
        RenderPass(const RenderPass&) = delete;
        RenderPass operator=(const RenderPass&) = delete;
        ~RenderPass();

        void RecreateRenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain);

        const VkRenderPass& GetRenderPass() const { return _renderPass; }
        
    private:
        VkRenderPass _renderPass;
        VkDevice& _logicalDevice;
        const RenderPassCreateInfo _createInfo;

    private:
        void DisposeVulkanObjects();
        void CreateRenderPass(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain);
    };
}
