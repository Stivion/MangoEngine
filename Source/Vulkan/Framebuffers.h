#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"

#include <vector>

#include<vulkan/vulkan.h>

namespace Mango 
{
    class Framebuffers
    {
    public:
        Framebuffers(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, Mango::RenderPass& renderPass);
        Framebuffers() = delete;
        Framebuffers(const Framebuffers&) = delete;
        Framebuffers operator=(const Framebuffers&) = delete;
        ~Framebuffers();
        
        const std::vector<VkFramebuffer>& GetSwapChainFramebuffers() const { return _swapChainFramebuffers; }
    private:
        std::vector<VkFramebuffer> _swapChainFramebuffers;
        VkDevice& _logicalDevice;
    };
}
