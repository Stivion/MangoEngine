#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>

namespace Mango
{
    struct QueueFamilyIndices
    {
        std::optional<std::uint32_t> GraphicsFamily;
        std::optional<std::uint32_t> PresentationFamily;

        static Mango::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface);
    };
}
