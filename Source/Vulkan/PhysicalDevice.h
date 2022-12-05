#pragma once

#include "QueueFamilyIndices.h"

#include <vulkan/vulkan.h>

namespace Mango
{

    class PhysicalDevice
    {
    public:
        PhysicalDevice() = default;
        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice operator=(const PhysicalDevice&) = delete;
        
        void PickPhysicalDevice(VkInstance* instance);
        QueueFamilyIndices FindQueueFamilies();
        
        VkPhysicalDevice& GetDevice() { return _device; }
        
    private:
        bool IsDeviceSuitable(VkPhysicalDevice& device);
        
    private:
        VkPhysicalDevice _device;
    };

}
