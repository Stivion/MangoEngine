#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Mango 
{
    struct SwapChainSupportDetails
    {
    public:
        VkSurfaceCapabilitiesKHR SurfaceCapabilities {};
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;

        static Mango::SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& renderSurface);
    };

}
