#pragma once

#include "QueueFamilyIndices.h"
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class CommandPool
    {
    public:
        CommandPool(const Mango::LogicalDevice& logicalDevice, const Mango::QueueFamilyIndices& queueFamilyIndices);
        CommandPool() = delete;
        CommandPool(const CommandPool&) = delete;
        CommandPool operator=(const CommandPool&) = delete;
        ~CommandPool();
        
        const VkCommandPool& GetCommandPool() const { return _commandPool; }

    private:
        VkCommandPool _commandPool;
        const VkDevice& _logicalDevice;
    };
}
