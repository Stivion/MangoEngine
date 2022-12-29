#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map>
#include <memory>

namespace Mango
{
	class DescriptorSetLayout;

	class DescriptorSetLayoutBuilder
	{
	public:
		DescriptorSetLayoutBuilder(const Mango::LogicalDevice& logicalDevice);

		Mango::DescriptorSetLayoutBuilder& AddBinding(uint64_t binding, VkDescriptorType descriptorType, VkShaderStageFlags shaderStages);
		std::unique_ptr<Mango::DescriptorSetLayout> Build();

	private:
		std::unordered_map<uint64_t, VkDescriptorSetLayoutBinding> _bindings;
		const Mango::LogicalDevice& _logicalDevice;
	};

	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout(const Mango::LogicalDevice& logicalDevice, std::vector<VkDescriptorSetLayoutBinding> bindings);
		DescriptorSetLayout() = delete;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout operator=(const DescriptorSetLayout&) = delete;
		~DescriptorSetLayout();

		const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return _layout; }
		const uint64_t GetId() const { return _descriptorSetId; }

	private:
		VkDescriptorSetLayout _layout;
		uint64_t _descriptorSetId; // TODO: Write some UUID algorithm
		const VkDevice& _logicalDevice;
	};
}