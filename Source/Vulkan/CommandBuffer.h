#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "CommandPool.h"

#include <cstdlib>

#include "vulkan/vulkan.h"

namespace Mango
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer operator=(const CommandBuffer&) = delete;
        
        void CreateCommandBuffer(LogicalDevice& logicalDevice, SwapChain& swapChain, RenderPass& renderPass, GraphicsPipeline& graphicsPipeline, Framebuffer& framebuffer, CommandPool& commandPool);
        void RecordCommandBuffer(uint32_t imageIndex);

        VkCommandBuffer& GetCommandBuffer() { return _commandBuffer; }
    private:
        VkCommandBuffer _commandBuffer;
        LogicalDevice* _logicalDevice;
        RenderPass* _renderPass;
        Framebuffer* _framebuffer;
        SwapChain* _swapChain;
        GraphicsPipeline* _graphicsPipeline;
    };
}
