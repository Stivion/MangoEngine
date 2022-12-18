#pragma once

#include "QueueFamilyIndices.h"
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class CommandPool
    {
    public:
        CommandPool(Mango::LogicalDevice& logicalDevice, Mango::QueueFamilyIndices& queueFamilyIndices);
        CommandPool() = delete;
        CommandPool(const CommandPool&) = delete;
        CommandPool operator=(const CommandPool&) = delete;
        ~CommandPool();
        
        const VkCommandPool& GetCommandPool() const { return _commandPool; }

    private:
        VkCommandPool _commandPool;
        VkDevice& _logicalDevice;
    };
}
