#include "SwapChainSupportDetails.h"

Mango::SwapChainSupportDetails Mango::SwapChainSupportDetails::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &renderSurface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, renderSurface, &details.SurfaceCapabilities);

    uint32_t formatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderSurface, &formatsCount, nullptr);
    if (formatsCount != 0)
    {
        details.Formats.resize(formatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderSurface, &formatsCount, details.Formats.data());
    }

    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderSurface, &presentModesCount, nullptr);
    if (presentModesCount != 0)
    {
        details.PresentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderSurface, &presentModesCount, details.PresentModes.data());
    }

    return details;
}