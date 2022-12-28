#include "RenderPass.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::RenderPass::RenderPass(const Mango::LogicalDevice& logicalDevice, const Mango::SwapChain& swapChain, const RenderPassCreateInfo& createInfo)
    : _logicalDevice(logicalDevice.GetDevice()), _createInfo(createInfo)
{
    CreateRenderPass(logicalDevice, swapChain);
}

Mango::RenderPass::~RenderPass()
{
    DisposeVulkanObjects();
}

void Mango::RenderPass::RecreateRenderPass(const Mango::LogicalDevice& logicalDevice, const Mango::SwapChain& swapChain)
{
    DisposeVulkanObjects();
    CreateRenderPass(logicalDevice, swapChain);
}

void Mango::RenderPass::DisposeVulkanObjects()
{
    vkDestroyRenderPass(_logicalDevice, _renderPass, nullptr);
}

void Mango::RenderPass::CreateRenderPass(const Mango::LogicalDevice& logicalDevice, const Mango::SwapChain& swapChain)
{
    const auto swapChainImageFormat = swapChain.GetSwapChainSurfaceFormat().format;
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = _createInfo.ColorAttachmentFinalLayout;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = _createInfo.ColorAttachmentReferenceLayout;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    auto renderPassCreateResult = vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass);
    M_TRACE("Render pass create result is: " + std::to_string(renderPassCreateResult));
    M_ASSERT(renderPassCreateResult == VK_SUCCESS && "Failed to create render pass");
}
