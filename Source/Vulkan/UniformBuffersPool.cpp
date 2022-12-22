#include "UniformBuffersPool.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::UniformBuffersPool::UniformBuffersPool(
	uint32_t uniformBuffersCount,
	Mango::PhysicalDevice& physicalDevice,
	Mango::LogicalDevice& logicalDevice,
	Mango::DescriptorPool& descriptorPool,
	Mango::DescriptorSetLayout& descriptorSetLayout
) : _logicalDevice(logicalDevice.GetDevice()), _layout(descriptorSetLayout.GetDescriptorSetLayout())
{
	_uniformBuffers.resize(uniformBuffersCount);
	for (size_t i = 0; i < uniformBuffersCount; i++)
	{
		auto uniformBuffer = new Mango::UniformBuffer(sizeof(UniformBufferObject), physicalDevice, logicalDevice);
		_uniformBuffers[i] = uniformBuffer;
	}

	std::vector<VkDescriptorSetLayout> layouts(uniformBuffersCount, _layout);
	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descriptorPool.GetDescriptorPool();
	allocateInfo.descriptorSetCount = static_cast<uint32_t>(uniformBuffersCount);
	allocateInfo.pSetLayouts = layouts.data();

	_descriptorSets.resize(uniformBuffersCount);
	auto allocateDescriptorSetsResult = vkAllocateDescriptorSets(_logicalDevice, &allocateInfo, _descriptorSets.data());
	M_TRACE("Allocate descriptor sets result is: " + std::to_string(allocateDescriptorSetsResult));
	M_ASSERT(allocateDescriptorSetsResult == VK_SUCCESS && "Failed to allocate descriptor sets");

	for (size_t i = 0; i < uniformBuffersCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _uniformBuffers[i]->GetBuffer();
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

Mango::UniformBuffersPool::~UniformBuffersPool()
{
	for (const auto& uniformBuffer : _uniformBuffers)
	{
		delete uniformBuffer;
	}
}
