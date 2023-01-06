#include "SwapChain.h"

#include "SwapChainSupportDetails.h"
#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>
#include <cstdint>
#include <limits>
#include <algorithm>

Mango::SwapChain::SwapChain(
    uint32_t windowFramebufferWidth,
    uint32_t windowFramebufferHeight,
    const Mango::RenderSurface& renderSurface,
    const Mango::LogicalDevice& logicalDevice,
    const Mango::SwapChainSupportDetails& swapChainSupportDetails,
    const Mango::QueueFamilyIndices& queueFamilyIndices
) : _logicalDevice(logicalDevice.GetDevice()), _renderSurface(renderSurface.GetRenderSurface())
{
    CreateSwapChain(
        windowFramebufferWidth,
        windowFramebufferHeight,
        _renderSurface,
        swapChainSupportDetails,
        queueFamilyIndices
    );
}

Mango::SwapChain::~SwapChain()
{
    DisposeVulkanObjects();
}

void Mango::SwapChain::RecreateSwapChain(
    uint32_t windowFramebufferWidth,
    uint32_t windowFramebufferHeight,
    const Mango::SwapChainSupportDetails& swapChainSupportDetails,
    const Mango::QueueFamilyIndices& queueFamilyIndices
)
{
    DisposeVulkanObjects();
    CreateSwapChain(
        windowFramebufferWidth,
        windowFramebufferHeight,
        _renderSurface,
        swapChainSupportDetails,
        queueFamilyIndices
    );
}

const VkResult Mango::SwapChain::AcquireNextImage(Mango::Semaphore& imageAvailableSemaphore)
{
    return vkAcquireNextImageKHR(
        _logicalDevice,
        _swapChain,
        UINT64_MAX,
        imageAvailableSemaphore.GetSemaphore(),
        VK_NULL_HANDLE,
        &_currentImageIndex
    );
}

void Mango::SwapChain::CreateSwapChain(
    uint32_t windowFramebufferWidth,
    uint32_t windowFramebufferHeight,
    const VkSurfaceKHR& renderSurface,
    const Mango::SwapChainSupportDetails& swapChainSupportDetails,
    const Mango::QueueFamilyIndices& queueFamilyIndices
)
{
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.Formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupportDetails.PresentModes);
    auto extent = ChooseSwapExtent(swapChainSupportDetails.SurfaceCapabilities, windowFramebufferWidth, windowFramebufferHeight);

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
    createInfo.surface = renderSurface;
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

    const auto createSwapChainResult = vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain);
    M_TRACE("Create swap chain result is: " + std::to_string(createSwapChainResult));
    M_ASSERT(createSwapChainResult == VK_SUCCESS && "Failed to create swap chain");

    _swapChainImages = CreateSwapChainImages();
    _swapChainImageViews = CreateImageViews(_swapChainImages);
}

void Mango::SwapChain::DisposeVulkanObjects()
{
    for (const auto& imageView : _swapChainImageViews)
    {
        vkDestroyImageView(_logicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);
}

std::vector<VkImage> Mango::SwapChain::CreateSwapChainImages()
{
    std::vector<VkImage> swapChainImages;
    uint32_t imageCount;

    VkResult getSwapChainImagesResult;
    getSwapChainImagesResult = vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
    M_TRACE("Swap chain image count is: " + std::to_string(imageCount));
    swapChainImages.resize(imageCount);
    getSwapChainImagesResult = vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, swapChainImages.data());
    M_ASSERT(getSwapChainImagesResult == VK_SUCCESS && "Failed to get swap chain images");
    return swapChainImages;
}

std::vector<VkImageView> Mango::SwapChain::CreateImageViews(std::vector<VkImage>& swapChainImages)
{
    std::vector<VkImageView> swapChainImageViews(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
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
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

VkExtent2D Mango::SwapChain::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    uint32_t windowFramebufferWidth,
    uint32_t windowFramebufferHeight
)
{
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = { windowFramebufferWidth, windowFramebufferHeight };
        actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        return actualExtent;
    }
}
