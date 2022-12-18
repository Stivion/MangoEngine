#include "CommandBuffer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandBuffer::CommandBuffer(
    Mango::LogicalDevice& logicalDevice,
    Mango::SwapChain& swapChain,
    Mango::RenderPass& renderPass,
    Mango::GraphicsPipeline& graphicsPipeline,
    Mango::Framebuffers& framebuffers,
    Mango::CommandPool& commandPool
) : _renderPass(renderPass.GetRenderPass()), _framebuffers(framebuffers.GetSwapChainFramebuffers()), _swapChain(swapChain), _graphicsPipeline(graphicsPipeline.GetGraphicsPipeline())
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    auto allocateCommandBuffersResult = vkAllocateCommandBuffers(logicalDevice.GetDevice(), &allocInfo, &_commandBuffer);
    M_TRACE("Allocate command buffers result is: " + std::to_string(allocateCommandBuffersResult));
    M_ASSERT(allocateCommandBuffersResult == VK_SUCCESS && "Failed to allocate command buffers");
}

Mango::CommandBuffer::~CommandBuffer()
{
}

void Mango::CommandBuffer::RecordCommandBuffer(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    auto beginCommandBufferResult = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
    M_ASSERT(beginCommandBufferResult == VK_SUCCESS && "Failed to begin recording command buffer");

    const auto& swapChainExtent = _swapChain.GetSwapChainExtent();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass;
    renderPassInfo.framebuffer = _framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

    vkCmdDraw(_commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(_commandBuffer);

    auto endCommandBufferResult = vkEndCommandBuffer(_commandBuffer);
    M_ASSERT(endCommandBufferResult == VK_SUCCESS && "Failed to record command buffer");
}
