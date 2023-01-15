#include "UniformBuffer.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

const VkBufferUsageFlags bufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
const VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

Mango::UniformBuffer::UniformBuffer(
	VkDeviceSize bufferSizeBytes,
	const Mango::PhysicalDevice& physicalDevice,
	const Mango::LogicalDevice& logicalDevice
) : Buffer(bufferUsageFlags, memoryFlags, GetAlignedSize(physicalDevice, bufferSizeBytes), physicalDevice, logicalDevice)
{
}

VkDeviceSize Mango::UniformBuffer::GetAlignedSize(const Mango::PhysicalDevice& physicalDevice, const VkDeviceSize bufferSizeBytes)
{
	const auto minOffsetAlignment = physicalDevice.GetLimits().minUniformBufferOffsetAlignment;
	VkDeviceSize alignedSize = bufferSizeBytes;
	if (minOffsetAlignment > 0)
	{
		alignedSize = (alignedSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}
	return alignedSize;
}

void* Mango::UniformBuffer::MapMemory() const
{
	return Buffer::MapMemory(_bufferSize);
}

VkDeviceSize Mango::UniformBuffer::GetAlignedSize(const VkDeviceSize bufferSizeBytes)
{
	return GetAlignedSize(_physicalDevice, bufferSizeBytes);
}

bool Mango::UniformBuffer::EnsureCapacity(VkDeviceSize bufferSizeBytes)
{
	if (_bufferSize >= bufferSizeBytes)
	{
		return false;
	}

	// Allocate 1.5x times more memory to minimaze memory allocations
	//ReallocateBuffer(bufferSizeBytes * 1.5f);
	ReallocateBuffer(bufferSizeBytes);
	return true;
}
