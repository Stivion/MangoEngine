#include "CommandBuffer.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandBuffer::CommandBuffer(
    VkCommandBuffer commandBuffer,
    const Mango::RenderPass& renderPass,
    const Mango::SwapChain& swapChain,
    const Mango::GraphicsPipeline& graphicsPipeline
) : _renderPass(renderPass), _swapChain(swapChain), _commandBuffer(commandBuffer), _graphicsPipeline(graphicsPipeline)
{
}

void Mango::CommandBuffer::BeginCommandBuffer() const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    auto beginCommandBufferResult = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
    M_ASSERT(beginCommandBufferResult == VK_SUCCESS && "Failed to begin recording command buffer");
}

void Mango::CommandBuffer::BeginRenderPass(const VkFramebuffer& framebuffer, const VkPipeline& graphicsPipeline, const ViewportInfo viewportInfo) const
{
    const auto& swapChainExtent = _swapChain.GetSwapChainExtent();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass.GetRenderPass();
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent; // NOTE: Should this match viewport size?

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = viewportInfo.X;
    viewport.y = viewportInfo.Y;
    viewport.width = viewportInfo.Width;
    viewport.height = viewportInfo.Height;
    viewport.minDepth = viewportInfo.MinDepth;
    viewport.maxDepth = viewportInfo.MaxDepth;
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent; // NOTE: Should this match viewport size?
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void Mango::CommandBuffer::DrawIndexed(
    const Mango::VertexBuffer& vertexBuffer,
    const Mango::IndexBuffer& indexBuffer,
    std::vector<VkDescriptorSet> descriptors
) const
{
    VkBuffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
    VkDeviceSize offsets[] = { 0 };
    auto descriptorSets = descriptors.data();
    auto descriptorSetsCount = static_cast<uint32_t>(descriptors.size());

    vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(_commandBuffer, indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(
        _commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _graphicsPipeline.GetPipelineLayout(),
        0,
        descriptorSetsCount,
        descriptorSets,
        0,
        nullptr
    );

    vkCmdDrawIndexed(_commandBuffer, indexBuffer.GetIndicesCount(), 1, 0, 0, 0);
}

void Mango::CommandBuffer::EndRenderPass() const
{
    vkCmdEndRenderPass(_commandBuffer);
}

void Mango::CommandBuffer::EndCommandBuffer() const
{
    auto endCommandBufferResult = vkEndCommandBuffer(_commandBuffer);
    M_ASSERT(endCommandBufferResult == VK_SUCCESS && "Failed to record command buffer");
}

void Mango::CommandBuffer::Reset() const
{
    vkResetCommandBuffer(_commandBuffer, 0);
}
