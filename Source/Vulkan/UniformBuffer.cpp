#include "UniformBuffer.h"

const VkBufferUsageFlags bufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
const VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

Mango::UniformBuffer::UniformBuffer(
	VkDeviceSize bufferSizeBytes,
	Mango::PhysicalDevice& physicalDevice,
	Mango::LogicalDevice& logicalDevice
) : Buffer(bufferUsageFlags, memoryFlags, bufferSizeBytes, physicalDevice, logicalDevice)
{
	_memoryPointer = MapMemory(bufferSizeBytes);
}

Mango::UniformBuffer::~UniformBuffer()
{
	UnmapMemory();
}
