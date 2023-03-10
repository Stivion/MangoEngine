#pragma once

#include "LogicalDevice.h"
#include "RenderPass.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace Mango 
{
    class Framebuffer
    {
    public:
        Framebuffer(
            const Mango::LogicalDevice& logicalDevice,
            const Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        Framebuffer() = delete;
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        void RecreateFramebuffer(
            const Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        
        const VkFramebuffer& GetSwapChainFramebuffer() const { return _swapChainFramebuffer; }

    private:
        VkFramebuffer _swapChainFramebuffer;
        const VkDevice& _logicalDevice;

    private:
        void CreateFramebuffer(
            const Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        void Dispose();
    };
}
