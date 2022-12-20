#include "DescriptorPool.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorPool::DescriptorPool(uint32_t descriptorsCount, Mango::DescriptorSetLayout& descriptorSetLayout, Mango::LogicalDevice& logicalDevice)
	: _logicalDevice(logicalDevice.GetDevice())
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = descriptorsCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = descriptorsCount;

	auto createDescriptorPoolResult = vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool);
	M_TRACE("Create descriptor pool resuls is: " + std::to_string(createDescriptorPoolResult));
	M_ASSERT(createDescriptorPoolResult == VK_SUCCESS && "Failed to create descriptor pool");

	std::vector<VkDescriptorSetLayout> layouts(descriptorsCount, descriptorSetLayout.GetDescriptorSetLayout());

	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = _descriptorPool;
	allocateInfo.descriptorSetCount = descriptorsCount;
	allocateInfo.pSetLayouts = layouts.data();

	_descriptorSets.resize(descriptorsCount);
	auto allocateDescriptorSetsResult = vkAllocateDescriptorSets(_logicalDevice, &allocateInfo, _descriptorSets.data());
	M_TRACE("Allocate descriptor sets result is: " + std::to_string(allocateDescriptorSetsResult));
	M_ASSERT(allocateDescriptorSetsResult == VK_SUCCESS && "Failed to allocate descriptor sets");
}

Mango::DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, nullptr);
}

void Mango::DescriptorPool::ConfigureForUniformBuffers(uint32_t buffersCount, Mango::UniformBuffer* buffers)
{
	for (size_t i = 0; i < buffersCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffers[i].GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeDescriptorInfo{};
		writeDescriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorInfo.dstSet = _descriptorSets[i];
		writeDescriptorInfo.dstBinding = 0;
		writeDescriptorInfo.dstArrayElement = 0;
		writeDescriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorInfo.descriptorCount = 1;
		writeDescriptorInfo.pBufferInfo = &bufferInfo;
		writeDescriptorInfo.pImageInfo = nullptr;
		writeDescriptorInfo.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(_logicalDevice, 1, &writeDescriptorInfo, 0, nullptr);
	}
}
