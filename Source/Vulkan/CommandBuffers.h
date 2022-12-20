#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffers.h"
#include "CommandPool.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
    class CommandBuffers
    {
    public:
        CommandBuffers(
            uint32_t commandBuffersCount,
            Mango::LogicalDevice& logicalDevice,
            Mango::SwapChain& swapChain,
            Mango::RenderPass& renderPass,
            Mango::GraphicsPipeline& graphicsPipeline,
            Mango::Framebuffers& framebuffers,
            Mango::CommandPool& commandPool
        );
        CommandBuffers() = delete;
        CommandBuffers(const CommandBuffers&) = delete;
        CommandBuffers operator=(const CommandBuffers&) = delete;
        
        void RecordCommandBuffer(
            const VkCommandBuffer& commandBuffer,
            const VkDescriptorSet& descriptorSet,
            const Mango::VertexBuffer& vertexBuffer,
            const Mango::IndexBuffer& indexBuffer,
            const  uint32_t imageIndex
        );
        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return _commandBuffers; }
        const VkCommandBuffer& GetCommandBuffer(uint32_t index) const { return _commandBuffers[index]; }
    private:
        std::vector<VkCommandBuffer> _commandBuffers;
        const VkRenderPass& _renderPass;
        const std::vector<VkFramebuffer>& _framebuffers;
        const Mango::SwapChain& _swapChain;
        const Mango::GraphicsPipeline& _graphicsPipeline;
    };
}
