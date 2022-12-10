#include "SwapChainSupportDetails.h"

Mango::SwapChainSupportDetails Mango::SwapChainSupportDetails::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &renderSurface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, renderSurface, &details.surfaceCapabilities);

    uint32_t formatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderSurface, &formatsCount, nullptr);
    if (formatsCount != 0)
    {
        details.formats.resize(formatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderSurface, &formatsCount, details.formats.data());
    }

    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderSurface, &presentModesCount, nullptr);
    if (presentModesCount != 0)
    {
        details.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderSurface, &presentModesCount, details.presentModes.data());
    }

    return details;
}