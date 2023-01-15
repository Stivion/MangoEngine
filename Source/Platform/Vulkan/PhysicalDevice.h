#pragma once

#include "Instance.h"
#include "RenderSurface.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Mango
{

    class PhysicalDevice
    {
    public:
        PhysicalDevice(Mango::Instance& instance, Mango::RenderSurface& renderSurface);
        PhysicalDevice() = delete;
        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice operator=(const PhysicalDevice&) = delete;

        uint32_t FindSuitableMemoryType(uint32_t memoryType, VkMemoryPropertyFlags requiredProperties) const;
        
        inline const VkPhysicalDevice& GetDevice() const { return _physicalDevice; }
        inline const VkPhysicalDeviceLimits& GetLimits() const { return _deviceLimits; }
    private:
        VkPhysicalDevice _physicalDevice;
        VkPhysicalDeviceLimits _deviceLimits;

    private:
        static bool IsDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface);
        static bool CheckDeviceExtensions(VkPhysicalDevice& device, const std::vector<const char*>& extensions);
    };

}
