#include "SwapChain.h"

#include "SwapChainSupportDetails.h"

#include <cstdint>
#include <limits>
#include <algorithm>

VkSurfaceFormatKHR Mango::SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
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

VkPresentModeKHR Mango::SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
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

void Mango::SwapChain::CreateSwapChain(GLFWwindow* window, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderSurface& renderSurface)
{
    _logicalDevice = &logicalDevice.GetDevice();
    auto swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(physicalDevice.GetDevice(), renderSurface.GetRenderSurface());
    
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupportDetails.presentModes);
    auto extent = ChooseSwapExtent(swapChainSupportDetails.surfaceCapabilities, window);
    
    auto imageCount = swapChainSupportDetails.surfaceCapabilities.minImageCount + 1;
    if (swapChainSupportDetails.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.surfaceCapabilities.maxImageCount) 
    {
        imageCount = swapChainSupportDetails.surfaceCapabilities.maxImageCount;
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

    auto indices = PhysicalDevice::FindQueueFamilies(physicalDevice.GetDevice(), renderSurface.GetRenderSurface());
    uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentationFamily.value()};

    if (indices.GraphicsFamily == indices.PresentationFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    createInfo.preTransform = swapChainSupportDetails.surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(*_logicalDevice, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }
    
    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
    vkGetSwapchainImagesKHR(*_logicalDevice, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());
}

Mango::SwapChain::~SwapChain()
{
    vkDestroySwapchainKHR(*_logicalDevice, _swapChain, nullptr);
}
