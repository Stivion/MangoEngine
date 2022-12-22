#include "DescriptorSetLayout.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorSetLayout::DescriptorSetLayout(Mango::LogicalDevice& logicalDevice)
	: _logicalDevice(logicalDevice.GetDevice())
{
	VkDescriptorSetLayoutBinding uniformBufferLayout{};
	uniformBufferLayout.binding = 0;
	uniformBufferLayout.descriptorCount = 1;
	uniformBufferLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferLayout.pImmutableSamplers = nullptr;
	uniformBufferLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uniformBufferLayout;

	auto createDescriptorSetLayoutResult = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, nullptr, &_layout);
	M_TRACE("Create descriptor set layout result is: " + std::to_string(createDescriptorSetLayoutResult));
	M_ASSERT(createDescriptorSetLayoutResult == VK_SUCCESS && "Failed to create descriptor set layout");
}

Mango::DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(_logicalDevice, _layout, nullptr);
}
