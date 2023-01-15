#include "Buffer.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Buffer::Buffer(
	VkBufferUsageFlags bufferUsage,
	VkMemoryPropertyFlags requiredProperties,
	VkDeviceSize bufferSizeBytes,
	const Mango::PhysicalDevice& physicalDevice,
	const Mango::LogicalDevice& logicalDevice
) : _logicalDevice(logicalDevice), _physicalDevice(physicalDevice), _bufferSize(bufferSizeBytes), _bufferUsage(bufferUsage), _requiredProperties(requiredProperties)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSizeBytes;
	bufferCreateInfo.usage = bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto createBufferResult = vkCreateBuffer(_logicalDevice.GetDevice(), &bufferCreateInfo, nullptr, &_buffer);
	M_TRACE("Create buffer result is: " + std::to_string(createBufferResult));
	M_ASSERT(createBufferResult == VK_SUCCESS && "Failed to create buffer");

	AllocateMemory(physicalDevice, requiredProperties);
}

Mango::Buffer::~Buffer()
{
	vkDestroyBuffer(_logicalDevice.GetDevice(), _buffer, nullptr);
	vkFreeMemory(_logicalDevice.GetDevice(), _bufferMemory, nullptr);
}

void Mango::Buffer::CopyToBuffer(uint32_t memorySizeBytes, const void* memoryBuffer) const
{
	void* gpuMemoryBuffer;
	auto mapResult = vkMapMemory(_logicalDevice.GetDevice(), _bufferMemory, 0, memorySizeBytes, 0, &gpuMemoryBuffer);
	M_TRACE("Map memory result is: " + std::to_string(mapResult));
	M_ASSERT(mapResult == VK_SUCCESS && "Failed to map memory for buffer");
	memcpy(gpuMemoryBuffer, memoryBuffer, static_cast<size_t>(memorySizeBytes));
	vkUnmapMemory(_logicalDevice.GetDevice(), _bufferMemory);
}

void* Mango::Buffer::MapMemory(uint32_t memorySizeBytes) const
{
	void* gpuMemoryBuffer;
	auto mapResult = vkMapMemory(_logicalDevice.GetDevice(), _bufferMemory, 0, memorySizeBytes, 0, &gpuMemoryBuffer);
	M_ASSERT(mapResult == VK_SUCCESS && "Failed to map memory for buffer");
	return gpuMemoryBuffer;
}

void Mango::Buffer::UnmapMemory() const
{
	vkUnmapMemory(_logicalDevice.GetDevice(), _bufferMemory);
}

void Mango::Buffer::ReallocateBuffer(VkDeviceSize bufferSizeBytes)
{
	vkDestroyBuffer(_logicalDevice.GetDevice(), _buffer, nullptr);
	vkFreeMemory(_logicalDevice.GetDevice(), _bufferMemory, nullptr);

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSizeBytes;
	bufferCreateInfo.usage = _bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto createBufferResult = vkCreateBuffer(_logicalDevice.GetDevice(), &bufferCreateInfo, nullptr, &_buffer);
	M_TRACE("Create buffer result is: " + std::to_string(createBufferResult));
	M_ASSERT(createBufferResult == VK_SUCCESS && "Failed to create buffer");

	AllocateMemory(_physicalDevice, _requiredProperties);
}

void Mango::Buffer::AllocateMemory(const Mango::PhysicalDevice& physicalDevice, VkMemoryPropertyFlags requiredProperties)
{
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(_logicalDevice.GetDevice(), _buffer, &memoryRequirements);

	const auto& memoryType = physicalDevice.FindSuitableMemoryType(
		memoryRequirements.memoryTypeBits,
		requiredProperties
	);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = memoryType;

	auto allocateResult = vkAllocateMemory(_logicalDevice.GetDevice(), &allocateInfo, nullptr, &_bufferMemory);
	M_TRACE("Allocate memory result is: " + std::to_string(allocateResult));
	M_ASSERT(allocateResult == VK_SUCCESS && "Failed to allocate buffer memory");

	auto bindResult = vkBindBufferMemory(_logicalDevice.GetDevice(), _buffer, _bufferMemory, 0);
	M_TRACE("Bind buffer memory result is: " + std::to_string(bindResult));
	M_ASSERT(bindResult == VK_SUCCESS && "Failed to bind buffer memory");
}
