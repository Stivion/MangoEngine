#pragma once

#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>

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

		void AllocateDescriptorSets(std::vector<const Mango::DescriptorSetLayout*> descriptorSetLayouts);

		const VkDescriptorPool& GetDescriptorPool() const { return _descriptorPool; }
		const VkDescriptorSet& GetDescriptorSet(uint64_t descriptorSetId) const { return _descriptorSets.at(descriptorSetId); }

	private:
		VkDescriptorPool _descriptorPool;
		std::unordered_map<uint64_t, VkDescriptorSet> _descriptorSets;
		const VkDevice& _logicalDevice;
	};
}