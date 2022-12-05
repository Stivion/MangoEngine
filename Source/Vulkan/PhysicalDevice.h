#pragma once

#include "QueueFamilyIndices.h"
#include "Instance.h"
#include "RenderSurface.h"

#include <vulkan/vulkan.h>

namespace Mango
{

    class PhysicalDevice
    {
    public:
        PhysicalDevice() = default;
        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice operator=(const PhysicalDevice&) = delete;
        
        void PickPhysicalDevice(Instance &instance, RenderSurface& renderSurface);
        static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface);
        
        VkPhysicalDevice& GetDevice() { return _device; }
        
    private:
        static bool IsDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface);
        
    private:
        VkPhysicalDevice _device;
    };

}
