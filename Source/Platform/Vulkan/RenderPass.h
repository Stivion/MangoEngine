#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    struct RenderPassCreateInfo
    {
        VkFormat ImageFormat;
        VkImageLayout ColorAttachmentReferenceLayout;
        VkImageLayout ColorAttachmentFinalLayout;
    };

    class RenderPass
    {
    public:
        RenderPass(const Mango::LogicalDevice& logicalDevice, const RenderPassCreateInfo& createInfo);
        RenderPass() = delete;
        RenderPass(const RenderPass&) = delete;
        RenderPass operator=(const RenderPass&) = delete;
        ~RenderPass();

        void RecreateRenderPass(const Mango::LogicalDevice& logicalDevice, VkFormat imageFormat);

        const VkRenderPass& GetRenderPass() const { return _renderPass; }
        
    private:
        VkRenderPass _renderPass;
        const VkDevice& _logicalDevice;
        const RenderPassCreateInfo _createInfo;

    private:
        void DisposeVulkanObjects();
        void CreateRenderPass(const Mango::LogicalDevice& logicalDevice, VkFormat imageFormat);
    };
}
