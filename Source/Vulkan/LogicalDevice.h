#pragma once

#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class LogicalDevice
    {
    public:
        LogicalDevice(const Mango::PhysicalDevice& physicalDevice, const Mango::QueueFamilyIndices& queueFamilyIndices);
        LogicalDevice() = delete;
        LogicalDevice(const LogicalDevice &) = delete;
        LogicalDevice operator=(const LogicalDevice &) = delete;
        ~LogicalDevice();

        const VkDevice& GetDevice() const { return _logicalDevice; }
        const VkQueue GetGraphicsQueue() const { return _graphicsQueue; }
        const VkQueue GetPresentationQueue() const { return _presentationQueue; }
    private:
        VkDevice _logicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentationQueue;
    };
}
