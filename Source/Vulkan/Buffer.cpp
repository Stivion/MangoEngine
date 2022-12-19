#include "Buffer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Buffer::Buffer(
	VkBufferUsageFlags bufferUsage,
	VkMemoryPropertyFlags requiredProperties,
	VkDeviceSize bufferSizeBytes,
	Mango::PhysicalDevice& physicalDevice,
	Mango::LogicalDevice& logicalDevice
) : _logicalDevice(logicalDevice.GetDevice())
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSizeBytes;
	bufferCreateInfo.usage = bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto createBufferResult = vkCreateBuffer(_logicalDevice, &bufferCreateInfo, nullptr, &_buffer);
	M_TRACE("Create buffer result is: " + std::to_string(createBufferResult));
	M_ASSERT(createBufferResult == VK_SUCCESS && "Failed to create buffer");

	AllocateMemory(physicalDevice, requiredProperties);
}

Mango::Buffer::~Buffer()
{
	vkDestroyBuffer(_logicalDevice, _buffer, nullptr);
	vkFreeMemory(_logicalDevice, _bufferMemory, nullptr);
}

void Mango::Buffer::MapMemory(uint32_t bufferSizeBytes, const void* memoryBuffer)
{
	void* gpuMemoryBuffer;
	vkMapMemory(_logicalDevice, _bufferMemory, 0, bufferSizeBytes, 0, &gpuMemoryBuffer);
	memcpy(gpuMemoryBuffer, memoryBuffer, static_cast<size_t>(bufferSizeBytes));
	vkUnmapMemory(_logicalDevice, _bufferMemory);
}

void Mango::Buffer::AllocateMemory(Mango::PhysicalDevice& physicalDevice, VkMemoryPropertyFlags requiredProperties)
{
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(_logicalDevice, _buffer, &memoryRequirements);

	const auto& memoryType = physicalDevice.FindSuitableMemoryType(
		memoryRequirements.memoryTypeBits,
		requiredProperties
	);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = memoryType;

	auto allocateResult = vkAllocateMemory(_logicalDevice, &allocateInfo, nullptr, &_bufferMemory);
	M_TRACE("Allocate memory result is: " + std::to_string(allocateResult));
	M_ASSERT(allocateResult == VK_SUCCESS && "Failed to allocate buffer memory");

	vkBindBufferMemory(_logicalDevice, _buffer, _bufferMemory, 0);
}
