#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "ImageView.h"

#include <vector>

#include<vulkan/vulkan.h>

namespace Mango 
{
    class Framebuffer
    {
    public:
        Framebuffer() = default;
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer operator=(const Framebuffer&) = delete;
        ~Framebuffer();
        
        void CreateFramebuffers(LogicalDevice& logicalDevice, SwapChain& swapChain, ImageView& imageView, RenderPass& renderPass);
    private:
        std::vector<VkFramebuffer> _swapChainFramebuffers;
        LogicalDevice* _logicalDevice;
    };
}
