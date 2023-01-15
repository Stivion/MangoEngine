#include "IndexBuffer.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

const VkBufferUsageFlags bufferUsageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
const VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

Mango::IndexBuffer::IndexBuffer(
	uint32_t indicesCount,
	VkDeviceSize bufferSizeBytes,
	const void* memory,
	const Mango::PhysicalDevice& physicalDevice,
	const Mango::LogicalDevice& logicalDevice,
	const Mango::CommandPool& commandPool
) : _indicesCount(indicesCount), Buffer(bufferUsageFlags, memoryFlags, bufferSizeBytes, physicalDevice, logicalDevice)
{
	Mango::Buffer stagingBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferSizeBytes,
		physicalDevice,
		logicalDevice
	);

	stagingBuffer.CopyToBuffer(bufferSizeBytes, memory);
	CopyToIndexBuffer(stagingBuffer.GetBuffer(), commandPool.GetCommandPool(), logicalDevice.GetGraphicsQueue(), bufferSizeBytes);
}

void Mango::IndexBuffer::CopyToIndexBuffer(
	const VkBuffer& sourceBuffer,
	const VkCommandPool& commandPool,
	const VkQueue& graphicsQueue,
	uint32_t bufferSizeBytes
)
{
	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = commandPool;
	allocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	auto allocateCommandBufferResult = vkAllocateCommandBuffers(_logicalDevice.GetDevice(), &allocateInfo, &commandBuffer);
	M_TRACE("Allocate command buffer result is: " + std::to_string(allocateCommandBufferResult));
	M_ASSERT(allocateCommandBufferResult == VK_SUCCESS && "Failed to allocate command buffer");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	auto beginResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	M_TRACE("Begin command buffer result is: " + std::to_string(beginResult));
	M_ASSERT(beginResult == VK_SUCCESS && "Failed to begin command buffer");

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSizeBytes;
	vkCmdCopyBuffer(commandBuffer, sourceBuffer, _buffer, 1, &copyRegion);
	auto endResult = vkEndCommandBuffer(commandBuffer);
	M_TRACE("End command buffer result is: " + std::to_string(endResult));
	M_ASSERT(endResult == VK_SUCCESS && "Failed to end command buffer");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	auto submitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	M_TRACE("Copy buffer submit result is: " + std::to_string(submitResult));
	M_ASSERT(submitResult == VK_SUCCESS && "Failed to submit buffer copy command");
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(_logicalDevice.GetDevice(), commandPool, 1, &commandBuffer);
}
