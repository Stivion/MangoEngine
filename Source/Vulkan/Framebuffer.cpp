﻿#include "Framebuffer.h"

Mango::Framebuffer::~Framebuffer()
{
    for (auto framebuffer : _swapChainFramebuffers) {
        vkDestroyFramebuffer(_logicalDevice->GetDevice(), framebuffer, nullptr);
    }
}

void Mango::Framebuffer::CreateFramebuffers(LogicalDevice& logicalDevice, SwapChain& swapChain, ImageView& imageView, RenderPass& renderPass)
{
    _logicalDevice = &logicalDevice;
    const auto& swapChainImageViews = imageView.GetImageViews();
    _swapChainFramebuffers.resize(swapChainImageViews.size());
    const auto& swapChainExtent = swapChain.GetSwapChainExtent();

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
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

        if (vkCreateFramebuffer(_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}
