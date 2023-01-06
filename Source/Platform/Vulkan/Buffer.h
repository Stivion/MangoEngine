#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class Buffer
	{
	public:
		Buffer(
			VkBufferUsageFlags bufferUsage,
			VkMemoryPropertyFlags requiredProperties,
			VkDeviceSize bufferSizeBytes,
			const Mango::PhysicalDevice& physicalDevice,
			const Mango::LogicalDevice& logicalDevice
		);
		Buffer(const Buffer&) = delete;
		Buffer operator=(const Buffer&) = delete;
		~Buffer();

		void CopyToBuffer(uint32_t memorySizeBytes, const void* memoryBuffer) const;
		void* MapMemory(uint32_t memorySizeBytes) const;
		void UnmapMemory() const;

		const VkBuffer& GetBuffer() const { return _buffer; }

	protected:
		VkBuffer _buffer;
		VkDeviceMemory _bufferMemory;
		VkDeviceSize _bufferSize;
		const VkDevice& _logicalDevice;

	private:
		void AllocateMemory(const Mango::PhysicalDevice& physicalDevice, VkMemoryPropertyFlags requiredProperties);
	};
}
