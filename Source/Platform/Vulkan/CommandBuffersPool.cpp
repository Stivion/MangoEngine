#include "CommandBuffersPool.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::CommandBuffersPool::CommandBuffersPool(
    uint32_t commandBuffersCount,
    const Mango::LogicalDevice& logicalDevice,
    const Mango::SwapChain& swapChain,
    const Mango::RenderPass& renderPass,
    const Mango::CommandPool& commandPool,
    const Mango::GraphicsPipeline& graphicsPipeline
)
{
    auto commandBuffers = new VkCommandBuffer[commandBuffersCount];

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBuffersCount;

    auto allocateCommandBuffersResult = vkAllocateCommandBuffers(logicalDevice.GetDevice(), &allocInfo, commandBuffers);
    M_TRACE("Allocate command buffers result is: " + std::to_string(allocateCommandBuffersResult));
    M_ASSERT(allocateCommandBuffersResult == VK_SUCCESS && "Failed to allocate command buffers");

    _commandBuffers.resize(commandBuffersCount);
    for (size_t i = 0; i < commandBuffersCount; i++)
    {
        Mango::CommandBuffer* commandBuffer = new Mango::CommandBuffer(commandBuffers[i], renderPass, swapChain, graphicsPipeline);
        _commandBuffers[i] = commandBuffer;
    }

    delete[] commandBuffers;
}

Mango::CommandBuffersPool::~CommandBuffersPool()
{
    for (const auto& commandBuffer : _commandBuffers)
    {
        delete commandBuffer;
    }
}
