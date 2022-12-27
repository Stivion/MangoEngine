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
			Mango::PhysicalDevice& physicalDevice,
			Mango::LogicalDevice& logicalDevice
		);
		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer operator=(const UniformBuffer&) = delete;
		~UniformBuffer() = default;

		void* MapMemory() const;
	};
}
