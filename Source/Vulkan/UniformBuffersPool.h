#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "UniformBuffer.h"
#include "UniformBufferObject.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Mango
{
	class UniformBuffersPool
	{
	public:
		UniformBuffersPool(
			uint32_t uniformBuffersCount,
			Mango::PhysicalDevice& physicalDevice,
			Mango::LogicalDevice& logicalDevice,
			Mango::DescriptorPool& descriptorPool,
			Mango::DescriptorSetLayout& descriptorSetLayout
		);
		UniformBuffersPool() = delete;
		UniformBuffersPool(const UniformBuffersPool&) = delete;
		UniformBuffersPool operator=(const UniformBuffersPool&) = delete;
		~UniformBuffersPool();
		
		Mango::UniformBuffer& GetUniformBuffer(uint32_t index) { return *_uniformBuffers[index]; }
		VkDescriptorSet& GetDescriptorSet(uint32_t index) { return _descriptorSets[index]; }

	private:
		std::vector<UniformBuffer*> _uniformBuffers;
		std::vector<VkDescriptorSet> _descriptorSets;
		VkDescriptorSetLayout& _layout;
		const VkDevice& _logicalDevice;
	};
}
