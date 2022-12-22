#include "Framebuffer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Framebuffer::Framebuffer(
    Mango::LogicalDevice& logicalDevice,
    Mango::RenderPass& renderPass,
    const VkExtent2D& extent,
    const VkImageView& imageView
) : _logicalDevice(logicalDevice.GetDevice())
{
    CreateFramebuffer(renderPass, extent, imageView);
}

Mango::Framebuffer::~Framebuffer()
{
    Dispose();
}

void Mango::Framebuffer::RecreateFramebuffer(
    Mango::RenderPass& renderPass,
    const VkExtent2D& extent,
    const VkImageView& imageView
)
{
    Dispose();
    CreateFramebuffer(renderPass, extent, imageView);
}

void Mango::Framebuffer::CreateFramebuffer(
    Mango::RenderPass& renderPass,
    const VkExtent2D& extent,
    const VkImageView& imageView
)
{
    VkImageView attachments[] = { imageView };
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass.GetRenderPass();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    auto createFramebufferResult = vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffer);
    M_TRACE("Create framebuffer result is: " + std::to_string(createFramebufferResult));
    M_ASSERT(createFramebufferResult == VK_SUCCESS && "Failed to create framebuffer");
}

void Mango::Framebuffer::Dispose()
{
    vkDestroyFramebuffer(_logicalDevice, _swapChainFramebuffer, nullptr);
}

