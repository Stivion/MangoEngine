#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Buffer.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class UniformBuffer : public Buffer
	{
	public:
		UniformBuffer(
			VkDeviceSize bufferSizeBytes,
			const Mango::PhysicalDevice& physicalDevice,
			const Mango::LogicalDevice& logicalDevice
		);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer operator=(const UniformBuffer&) = delete;
		~UniformBuffer() = default;

		void* MapMemory() const;
		VkDeviceSize GetAlignedSize(const VkDeviceSize bufferSizeBytes);
		bool EnsureCapacity(VkDeviceSize bufferSizeBytes);

	private:
		VkDeviceSize GetAlignedSize(const Mango::PhysicalDevice& physicalDevice, const VkDeviceSize bufferSizeBytes);
	};
}
