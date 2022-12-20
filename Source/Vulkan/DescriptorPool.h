#pragma once

#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"
#include "UniformBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class DescriptorPool
	{
	public:
		DescriptorPool(uint32_t descriptorsCount, Mango::DescriptorSetLayout& descriptorSetLayout, Mango::LogicalDevice& logicalDevice);
		DescriptorPool() = delete;
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool operator=(const DescriptorPool&) = delete;
		~DescriptorPool();

		void ConfigureForUniformBuffers(uint32_t buffersCount, Mango::UniformBuffer* buffers);

		const VkDescriptorPool& GetDescriptorPool() const { return _descriptorPool; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() { return _descriptorSets; }

	private:
		VkDescriptorPool _descriptorPool;
		std::vector<VkDescriptorSet> _descriptorSets;
		const VkDevice& _logicalDevice;
	};
}