#include "DescriptorSetLayout.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorSetLayout::DescriptorSetLayout(Mango::LogicalDevice& logicalDevice) : _logicalDevice(logicalDevice.GetDevice())
{
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &layoutBinding;

	auto createDescriptorSetLayoutResult = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout);
	M_TRACE("Create descriptor set layout result is: " + std::to_string(createDescriptorSetLayoutResult));
	M_ASSERT(createDescriptorSetLayoutResult == VK_SUCCESS && "Failed to create descriptor set layout");
}

Mango::DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(_logicalDevice, _descriptorSetLayout, nullptr);
}
