#pragma once

#include "LogicalDevice.h"
#include "UniformBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class DescriptorPool
	{
	public:
		DescriptorPool(
			const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
			Mango::LogicalDevice& logicalDevice
		);
		DescriptorPool() = delete;
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool operator=(const DescriptorPool&) = delete;
		~DescriptorPool();

		const VkDescriptorPool& GetDescriptorPool() const { return _descriptorPool; }

	private:
		VkDescriptorPool _descriptorPool;
		const VkDevice& _logicalDevice;
	};
}