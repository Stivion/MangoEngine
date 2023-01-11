#include "CommandBuffer.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer, const Mango::RenderPass& renderPass) 
    : _renderPass(renderPass), _commandBuffer(commandBuffer)
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

void Mango::CommandBuffer::BeginRenderPass(const VkFramebuffer& framebuffer, const Mango::RenderArea renderArea) const
{
    // TODO: Main refactor here
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass.GetRenderPass();
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { renderArea.X, renderArea.Y };
    renderPassInfo.renderArea.extent.width = renderArea.Width;
    renderPassInfo.renderArea.extent.height = renderArea.Height;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = renderArea.X;
    viewport.y = renderArea.Y;
    viewport.width = renderArea.Width;
    viewport.height = renderArea.Height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { renderArea.X, renderArea.Y };
    scissor.extent.width = renderArea.Width;
    scissor.extent.height = renderArea.Height;
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
}

void Mango::CommandBuffer::DrawIndexed(
    const Mango::VertexBuffer& vertexBuffer,
    const Mango::IndexBuffer& indexBuffer,
    std::vector<VkDescriptorSet> descriptors,
    const VkPipelineLayout& pipelineLayout
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
        pipelineLayout,
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

void Mango::CommandBuffer::BindPipeline(const Mango::GraphicsPipeline& pipeline) const
{
    pipeline.GetPipelineLayout();
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetGraphicsPipeline());
}
