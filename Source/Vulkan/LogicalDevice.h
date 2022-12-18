#pragma once

#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class LogicalDevice
    {
    public:
        LogicalDevice(Mango::PhysicalDevice& physicalDevice, Mango::QueueFamilyIndices& queueFamilyIndices);
        LogicalDevice() = delete;
        LogicalDevice(const LogicalDevice &) = delete;
        LogicalDevice operator=(const LogicalDevice &) = delete;
        ~LogicalDevice();

        VkDevice& GetDevice() { return _logicalDevice; }
        VkQueue GetGraphicsQueue() { return _graphicsQueue; }
        VkQueue GetPresentationQueue() { return _presentationQueue; }
    private:
        VkDevice _logicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentationQueue;
    };
}
