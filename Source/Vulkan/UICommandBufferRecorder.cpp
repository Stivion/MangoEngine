#include "UICommandBufferRecorder.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"

Mango::UICommandBufferRecorder::UICommandBufferRecorder(UICommandBufferRecorderInfo recorderInfo)
    : _physicalDevice(recorderInfo.PhysicalDevice),
    _logicalDevice(recorderInfo.LogicalDevice),
    _graphicsPipeline(recorderInfo.GraphicsPipeline),
    _swapChain(recorderInfo.SwapChain),
    _viewportImage(recorderInfo.ViewportImage),
    _viewportInfo(recorderInfo.ViewportInfo)
{
    _firstTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_NONE,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );
    _secondTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT
    );
}

const void Mango::UICommandBufferRecorder::RecordCommandBuffer(
    const Mango::CommandBuffer* commandBuffer,
    const Mango::Framebuffer* framebuffer
) const
{
    const auto& currentExtent = _swapChain->GetSwapChainExtent();
    const auto& vkCommandBuffer = commandBuffer->GetVkCommandBuffer();

    commandBuffer->Reset();
    commandBuffer->BeginCommandBuffer();

    vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &_firstTransitionBarrier);
    const VkDeviceSize imageSize = static_cast<VkDeviceSize>(currentExtent.width) * currentExtent.height * 4;

    VkImageSubresourceLayers srcSubResourceLayer{};
    srcSubResourceLayer.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    srcSubResourceLayer.baseArrayLayer = 0;
    srcSubResourceLayer.layerCount = 1;
    srcSubResourceLayer.mipLevel = 0;

    VkImageCopy region{};
    region.srcOffset = { 0, 0, 0 };
    region.dstOffset = { 0, 0, 0 };
    region.extent.width = currentExtent.width;
    region.extent.height = currentExtent.height;
    region.extent.depth = 1;
    region.srcSubresource = srcSubResourceLayer;
    region.dstSubresource = srcSubResourceLayer;
    vkCmdCopyImage(
        vkCommandBuffer,
        _swapChain->GetCurrentSwapChainImage(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        _viewportImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &_secondTransitionBarrier);

    commandBuffer->BeginRenderPass(
        framebuffer->GetSwapChainFramebuffer(),
        _graphicsPipeline->GetGraphicsPipeline(),
        _viewportInfo
    );

    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer->GetVkCommandBuffer());

    commandBuffer->EndRenderPass();
    commandBuffer->EndCommandBuffer();
}

VkImageMemoryBarrier Mango::UICommandBufferRecorder::CreateImageMemoryBarrier(
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask
)
{
    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.image = _viewportImage;
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = 1;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    return memoryBarrier;
}
