#pragma once

#include "RenderSurface.h"
#include "LogicalDevice.h"
#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"
#include "Semaphore.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
    class SwapChain
    {
    public:
        SwapChain(
            uint32_t windowFramebufferWidth,
            uint32_t windowFramebufferHeight,
            const Mango::RenderSurface& renderSurface,
            const Mango::LogicalDevice& logicalDevice,
            const Mango::SwapChainSupportDetails& swapChainSupportDetails,
            const Mango::QueueFamilyIndices& queueFamilyIndices
        );
        SwapChain() = delete;
        SwapChain(const SwapChain&) = delete;
        SwapChain operator=(const SwapChain&) = delete;
        ~SwapChain();

        void RecreateSwapChain(
            uint32_t windowFramebufferWidth,
            uint32_t windowFramebufferHeight,
            const Mango::SwapChainSupportDetails& swapChainSupportDetails,
            const Mango::QueueFamilyIndices& queueFamilyIndices
        );

        const VkResult AcquireNextImage(Mango::Semaphore& imageAvailableSemaphore);
        const uint32_t GetCurrentImageIndex() const { return _currentImageIndex; };
        const VkSwapchainKHR& GetSwapChain() const { return _swapChain; }
        const VkExtent2D& GetSwapChainExtent() const { return _swapChainExtent; }
        const VkSurfaceFormatKHR& GetSwapChainSurfaceFormat() const { return _swapChainSurfaceFormat; }
        const VkPresentModeKHR& GetSwapChainPresentMode() const { return _swapChainPresentMode; }
        const std::vector<VkImageView>& GetSwapChainImageViews() const { return _swapChainImageViews; }
        const std::vector<VkImage>& GetSwapChainImages() const { return _swapChainImages; }
        const VkImage& GetCurrentSwapChainImage() const { return _swapChainImages[_currentImageIndex]; }
    private:
        // Keep reference to RenderSurface to recreate swap chain if we need to
        const VkSurfaceKHR& _renderSurface;
        const VkDevice& _logicalDevice;

        VkSwapchainKHR _swapChain;
        uint32_t _currentImageIndex;
        VkExtent2D _swapChainExtent;
        VkSurfaceFormatKHR _swapChainSurfaceFormat;
        VkPresentModeKHR _swapChainPresentMode;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;
    private:
        std::vector<VkImageView> CreateImageViews(std::vector<VkImage>& swapChainImages);

        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        static VkExtent2D ChooseSwapExtent(
            const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
            uint32_t windowFramebufferWidth,
            uint32_t windowFramebufferHeight
        );

        void DisposeVulkanObjects();
        std::vector<VkImage> CreateSwapChainImages();
        void CreateSwapChain(
            uint32_t windowFramebufferWidth,
            uint32_t windowFramebufferHeight,
            const VkSurfaceKHR& renderSurface,
            const Mango::SwapChainSupportDetails& swapChainSupportDetails,
            const Mango::QueueFamilyIndices& queueFamilyIndices
        );
    };
}
