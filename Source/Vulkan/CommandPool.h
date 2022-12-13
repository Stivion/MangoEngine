#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "RenderSurface.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class CommandPool
    {
    public:
        CommandPool() = default;
        CommandPool(const CommandPool&) = delete;
        CommandPool operator=(const CommandPool&) = delete;
        ~CommandPool();
        
        void CreateCommandPool(PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderSurface& renderSurface);
        VkCommandPool& GetCommandPool() { return _commandPool; }
    private:
        LogicalDevice* _logicalDevice;
        VkCommandPool _commandPool;
    };
}
