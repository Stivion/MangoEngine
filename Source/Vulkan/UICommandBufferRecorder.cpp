#include "UICommandBufferRecorder.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"

Mango::UICommandBufferRecorder::UICommandBufferRecorder(UICommandBufferRecorderInfo recorderInfo)
    : _graphicsPipeline(recorderInfo.GraphicsPipeline), _viewportInfo(recorderInfo.ViewportInfo)
{
}

const void Mango::UICommandBufferRecorder::RecordCommandBuffer(
    const Mango::CommandBuffer* commandBuffer,
    const Mango::Framebuffer* framebuffer
) const
{
    //const auto& vkCommandBuffer = commandBuffer->GetVkCommandBuffer();

    commandBuffer->BeginCommandBuffer();
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
