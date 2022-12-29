#include "CommandPool.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandPool::CommandPool(const Mango::LogicalDevice& logicalDevice, const Mango::QueueFamilyIndices& queueFamilyIndices)
    : _logicalDevice(logicalDevice.GetDevice())
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

    auto createCommandPoolResult = vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool);
    M_TRACE("Create command pool result is: " + std::to_string(createCommandPoolResult));
    M_ASSERT(createCommandPoolResult == VK_SUCCESS && "Failed to create command pool");
}

Mango::CommandPool::~CommandPool()
{
    vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);
}
