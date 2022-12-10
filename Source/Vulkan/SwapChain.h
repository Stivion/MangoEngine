#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "RenderSurface.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Mango
{
    class SwapChain
    {
    public:
        SwapChain() = default;
        SwapChain(const SwapChain&) = delete;
        SwapChain operator=(const SwapChain&) = delete;
        ~SwapChain();
        
        void CreateSwapChain(GLFWwindow* window, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderSurface& renderSurface);

        VkSwapchainKHR& GetSwapChain() { return _swapChain; }
        std::vector<VkImage>& GetSwapChainImages() { return _swapChainImages; }
        VkFormat& GetSwapChainImageFormat() { return _swapChainImageFormat; }
        VkExtent2D& GetSwapChainExtent() { return _swapChainExtent; }
    public:
    private:
        VkDevice* _logicalDevice;
        VkSwapchainKHR _swapChain;
        std::vector<VkImage> _swapChainImages;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
    private:
        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window);
    };
}
