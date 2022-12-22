#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout(Mango::LogicalDevice& logicalDevice);
		DescriptorSetLayout() = delete;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout operator=(const DescriptorSetLayout&) = delete;
		~DescriptorSetLayout();

		VkDescriptorSetLayout& GetDescriptorSetLayout() { return _layout; }

	private:
		VkDescriptorSetLayout _layout;
		const VkDevice& _logicalDevice;
	};
}