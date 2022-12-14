#pragma once

#include "PhysicalDevice.h"
#include "RenderSurface.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class LogicalDevice
    {
    public:
        LogicalDevice() = default;
        LogicalDevice(const LogicalDevice &) = delete;
        LogicalDevice operator=(const LogicalDevice &) = delete;
        ~LogicalDevice();

        void CreateLogicalDevice(PhysicalDevice& physicalDevice, RenderSurface& renderSurface);

        VkDevice& GetDevice() { return _device; }
        VkQueue& GetGraphicsQueue() { return _graphicsQueue; }
        VkQueue& GetPresentationQueue() { return _presentationQueue; }

    private:
        VkDevice _device;
        VkQueue _graphicsQueue;
        VkQueue _presentationQueue;
    };
}
