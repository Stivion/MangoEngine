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
		const VkDeviceSize GetSize() const { return _bufferSize; }

	protected:
		void ReallocateBuffer(VkDeviceSize bufferSizeBytes);

	protected:
		VkBufferUsageFlags _bufferUsage;
		VkMemoryPropertyFlags _requiredProperties;
		VkBuffer _buffer;
		VkDeviceMemory _bufferMemory;
		VkDeviceSize _bufferSize;
		const Mango::LogicalDevice& _logicalDevice;
		const Mango::PhysicalDevice& _physicalDevice;

	private:
		void AllocateMemory(const Mango::PhysicalDevice& physicalDevice, VkMemoryPropertyFlags requiredProperties);
	};
}
