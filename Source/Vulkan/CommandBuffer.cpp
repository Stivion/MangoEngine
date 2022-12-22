#include "CommandBuffer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandBuffer::CommandBuffer(
    VkCommandBuffer commandBuffer,
    Mango::RenderPass& renderPass,
    Mango::SwapChain& swapChain,
    Mango::GraphicsPipeline& graphicsPipeline
) : _renderPass(renderPass), _swapChain(swapChain), _graphicsPipeline(graphicsPipeline), _commandBuffer(commandBuffer)
{
}

void Mango::CommandBuffer::BeginCommandBuffer(const VkFramebuffer& framebuffer)
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
    renderPassInfo.renderPass = _renderPass.GetRenderPass();
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.GetGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void Mango::CommandBuffer::DrawIndexed(
    const Mango::VertexBuffer& vertexBuffer,
    const Mango::IndexBuffer& indexBuffer,
    const VkDescriptorSet& descriptorSet
)
{
    VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_commandBuffer, indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(_commandBuffer, indexBuffer.GetIndicesCount(), 1, 0, 0, 0);
}

void Mango::CommandBuffer::EndCommandBuffer()
{
    vkCmdEndRenderPass(_commandBuffer);
    auto endCommandBufferResult = vkEndCommandBuffer(_commandBuffer);
    M_ASSERT(endCommandBufferResult == VK_SUCCESS && "Failed to record command buffer");
}
