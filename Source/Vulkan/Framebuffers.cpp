#include "Framebuffers.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Framebuffers::Framebuffers(
    Mango::LogicalDevice& logicalDevice,
    Mango::SwapChain& swapChain,
    Mango::RenderPass& renderPass
) : _logicalDevice(logicalDevice.GetDevice())
{
    CreateFramebuffers(logicalDevice, swapChain, renderPass);
}

Mango::Framebuffers::~Framebuffers()
{
    DisposeVulkanObjects();
}

void Mango::Framebuffers::RecreateFramebuffers(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, Mango::RenderPass& renderPass)
{
    DisposeVulkanObjects();
    CreateFramebuffers(logicalDevice, swapChain, renderPass);
}

void Mango::Framebuffers::DisposeVulkanObjects()
{
    for (const auto& framebuffer : _swapChainFramebuffers)
    {
        vkDestroyFramebuffer(_logicalDevice, framebuffer, nullptr);
    }
}

void Mango::Framebuffers::CreateFramebuffers(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, Mango::RenderPass& renderPass)
{
    const auto& swapChainImageViews = swapChain.GetSwapChainImageViews();
    _swapChainFramebuffers.resize(swapChainImageViews.size());
    const auto& swapChainExtent = swapChain.GetSwapChainExtent();

    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] =
        {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        auto createFramebufferResult = vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]);
        M_TRACE("Create framebuffer result is: " + std::to_string(createFramebufferResult));
        M_ASSERT(createFramebufferResult == VK_SUCCESS && "Failed to create framebuffer");
    }
}
