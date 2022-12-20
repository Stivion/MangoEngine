#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

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

        const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return _descriptorSetLayout; }

    private:
        VkDescriptorSetLayout _descriptorSetLayout;
        const VkDevice& _logicalDevice;
	};
}
