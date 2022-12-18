#include "QueueFamilyIndices.h"

Mango::QueueFamilyIndices Mango::QueueFamilyIndices::FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface)
{
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    auto* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (size_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
    {
        // Check for graphics support
        if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = queueFamilyIndex;
        }

        // Check for presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, renderSurface, &presentSupport);
        if (presentSupport)
        {
            indices.PresentationFamily = queueFamilyIndex;
        }


        if (indices.GraphicsFamily.has_value() && indices.PresentationFamily.has_value())
        {
            break;
        }
    }

    delete[] queueFamilies;
    return indices;
}
