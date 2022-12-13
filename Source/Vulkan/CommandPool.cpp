#include "CommandPool.h"

Mango::CommandPool::~CommandPool()
{
    vkDestroyCommandPool(_logicalDevice->GetDevice(), _commandPool, nullptr);
}

void Mango::CommandPool::CreateCommandPool(PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderSurface& renderSurface)
{
    _logicalDevice = &logicalDevice;

    const auto& queueFamilyIndices = PhysicalDevice::FindQueueFamilies(physicalDevice.GetDevice(), renderSurface.GetRenderSurface());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

    if (vkCreateCommandPool(_logicalDevice->GetDevice(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
}
