#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffers.h"
#include "CommandPool.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class CommandBuffer
    {
    public:
        CommandBuffer(
            Mango::LogicalDevice& logicalDevice,
            Mango::SwapChain& swapChain,
            Mango::RenderPass& renderPass,
            Mango::GraphicsPipeline& graphicsPipeline,
            Mango::Framebuffers& framebuffers,
            Mango::CommandPool& commandPool
        );
        CommandBuffer() = delete;
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer operator=(const CommandBuffer&) = delete;
        ~CommandBuffer();
        
        void RecordCommandBuffer(uint32_t imageIndex);
        const VkCommandBuffer& GetCommandBuffer() const { return _commandBuffer; }
    private:
        VkCommandBuffer _commandBuffer;
        const VkRenderPass& _renderPass;
        const std::vector<VkFramebuffer>& _framebuffers;
        const Mango::SwapChain& _swapChain;
        const VkPipeline& _graphicsPipeline;
    };
}
