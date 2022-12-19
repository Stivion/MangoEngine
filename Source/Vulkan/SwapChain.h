#pragma once

#include "../Windowing/Window.h"
#include "RenderSurface.h"
#include "LogicalDevice.h"
#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Mango
{
    class SwapChain
    {
    public:
        SwapChain(
            Mango::Window& window,
            Mango::RenderSurface& renderSurface,
            Mango::LogicalDevice& logicalDevice,
            Mango::SwapChainSupportDetails& swapChainSupportDetails,
            Mango::QueueFamilyIndices& queueFamilyIndices
        );
        SwapChain() = delete;
        SwapChain(const SwapChain&) = delete;
        SwapChain operator=(const SwapChain&) = delete;
        ~SwapChain();

        void RecreateSwapChain(Mango::SwapChainSupportDetails& swapChainSupportDetails, Mango::QueueFamilyIndices& queueFamilyIndices);

        VkSwapchainKHR& GetSwapChain() { return _swapChain; }
        const VkExtent2D& GetSwapChainExtent() const { return _swapChainExtent; }
        const VkSurfaceFormatKHR& GetSwapChainSurfaceFormat() const { return _swapChainSurfaceFormat; }
        const VkPresentModeKHR& GetSwapChainPresentMode() const { return _swapChainPresentMode; }
        const std::vector<VkImageView>& GetSwapChainImageViews() const { return _swapChainImageViews; }
        const std::vector<VkImage>& GetSwapChainImages() const { return _swapChainImages; }
    private:
        // Keep reference to Window and RenderSurface to recreate swap chain if we need to, this values will not change
        GLFWwindow* _window;
        VkSurfaceKHR& _renderSurface;
        // Keep reference to LogicalDevice to destroy swap chain in destructor
        VkDevice& _logicalDevice;

        VkSwapchainKHR _swapChain;
        VkExtent2D _swapChainExtent;
        VkSurfaceFormatKHR _swapChainSurfaceFormat;
        VkPresentModeKHR _swapChainPresentMode;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;
    private:
        std::vector<VkImageView> CreateImageViews(std::vector<VkImage>& swapChainImages);

        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window);

        void DisposeVulkanObjects();
        std::vector<VkImage> CreateSwapChainImages();
        void CreateSwapChain(
            GLFWwindow* window,
            VkSurfaceKHR& renderSurface,
            Mango::SwapChainSupportDetails& swapChainSupportDetails,
            Mango::QueueFamilyIndices& queueFamilyIndices
        );
    };
}
