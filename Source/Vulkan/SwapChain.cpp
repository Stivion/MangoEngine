﻿#include "SwapChain.h"

#include "SwapChainSupportDetails.h"
#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>
#include <vector>
#include <cstdint>
#include <limits>
#include <algorithm>

Mango::SwapChain::SwapChain(
    Mango::Window& window,
    Mango::RenderSurface& renderSurface,
    Mango::LogicalDevice& logicalDevice,
    Mango::SwapChainSupportDetails& swapChainSupportDetails,
    Mango::QueueFamilyIndices& queueFamilyIndices
) : _logicalDevice(logicalDevice.GetDevice())
{
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.Formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupportDetails.PresentModes);
    auto extent = ChooseSwapExtent(swapChainSupportDetails.SurfaceCapabilities, window.GetWindow());

    _swapChainSurfaceFormat = surfaceFormat;
    _swapChainPresentMode = presentMode;
    _swapChainExtent = extent;

    auto imageCount = swapChainSupportDetails.SurfaceCapabilities.minImageCount + 1;
    if (swapChainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.SurfaceCapabilities.maxImageCount)
    {
        imageCount = swapChainSupportDetails.SurfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = renderSurface.GetRenderSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::vector<uint32_t> indices = { queueFamilyIndices.GraphicsFamily.value(), queueFamilyIndices.PresentationFamily.value() };
    if (queueFamilyIndices.GraphicsFamily == queueFamilyIndices.PresentationFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = indices.size();
        createInfo.pQueueFamilyIndices = indices.data();
    }

    createInfo.preTransform = swapChainSupportDetails.SurfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    const auto createSwapChainResult = vkCreateSwapchainKHR(logicalDevice.GetDevice(), &createInfo, nullptr, &_swapChain);
    M_TRACE("Create swap chain result is: " + std::to_string(createSwapChainResult));
    M_ASSERT(createSwapChainResult == VK_SUCCESS && "Failed to create swap chain");

    VkResult getSwapChainImagesResult;
    getSwapChainImagesResult = vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
    M_TRACE("Swap chain image count is: " + std::to_string(imageCount));
    _swapChainImages.resize(imageCount);
    getSwapChainImagesResult = vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());
    M_ASSERT(getSwapChainImagesResult == VK_SUCCESS && "Failed to get swap chain images");

    _swapChainImageViews = CreateImageViews();
}

Mango::SwapChain::~SwapChain()
{
    for (const auto& imageView : _swapChainImageViews) 
    {
        vkDestroyImageView(_logicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);
}

std::vector<VkImageView> Mango::SwapChain::CreateImageViews()
{
    std::vector<VkImageView> swapChainImageViews(_swapChainImages.size());
    for (size_t i = 0; i < _swapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _swapChainSurfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        auto createImageViewResult = vkCreateImageView(_logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]);
        M_TRACE("Create swap chain image view result is: " + std::to_string(createImageViewResult));
        M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create image view");
    }
    return swapChainImageViews;
}

VkSurfaceFormatKHR Mango::SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Mango::SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Mango::SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window)
{
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = 
        {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}
