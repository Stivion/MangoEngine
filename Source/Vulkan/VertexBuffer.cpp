#include "VertexBuffer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::VertexBuffer::VertexBuffer(
	uint32_t vertexCount,
	uint32_t bufferSizeBytes,
	const void* memory,
	Mango::PhysicalDevice& physicalDevice,
	Mango::LogicalDevice& logicalDevice
) : _logicalDevice(logicalDevice.GetDevice()), _vertexCount(vertexCount)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSizeBytes;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto createBufferResult = vkCreateBuffer(_logicalDevice, &bufferCreateInfo, nullptr, &_buffer);
	M_TRACE("Create vertex buffer result is: " + std::to_string(createBufferResult));
	M_ASSERT(createBufferResult == VK_SUCCESS && "Failed to create vertex buffer");

	AllocateMemory(physicalDevice);
	MapMemory(bufferSizeBytes, memory);
}

Mango::VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(_logicalDevice, _buffer, nullptr);
	vkFreeMemory(_logicalDevice, _vertexBufferMemory, nullptr);
}

void Mango::VertexBuffer::AllocateMemory(Mango::PhysicalDevice& physicalDevice)
{
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(_logicalDevice, _buffer, &memoryRequirements);

	const auto& memoryType = physicalDevice.FindSuitableMemoryType(
		memoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = memoryType;

	auto allocateResult = vkAllocateMemory(_logicalDevice, &allocateInfo, nullptr, &_vertexBufferMemory);
	M_TRACE("Allocate memory result is: " + std::to_string(allocateResult));
	M_ASSERT(allocateResult == VK_SUCCESS && "Failed to allocate vertex buffer memory");

	vkBindBufferMemory(_logicalDevice, _buffer, _vertexBufferMemory, 0);
}

void Mango::VertexBuffer::MapMemory(uint32_t bufferSizeBytes, const void* memoryBuffer)
{
	void* gpuMemoryBuffer;
	vkMapMemory(_logicalDevice, _vertexBufferMemory, 0, bufferSizeBytes, 0, &gpuMemoryBuffer);
	memcpy(gpuMemoryBuffer, memoryBuffer, static_cast<size_t>(bufferSizeBytes));
	vkUnmapMemory(_logicalDevice, _vertexBufferMemory);
}
