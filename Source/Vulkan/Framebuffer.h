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
            Mango::LogicalDevice& logicalDevice,
            Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        Framebuffer() = delete;
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        void RecreateFramebuffer(
            Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        
        const VkFramebuffer& GetSwapChainFramebuffer() { return _swapChainFramebuffer; }

    private:
        VkFramebuffer _swapChainFramebuffer;
        VkDevice& _logicalDevice;

    private:
        void CreateFramebuffer(
            Mango::RenderPass& renderPass,
            const VkExtent2D& extent,
            const VkImageView& imageView
        );
        void Dispose();
    };
}
