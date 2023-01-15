#include "UniformBuffersPool.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::UniformBuffersPool::UniformBuffersPool(const Mango::PhysicalDevice& physicalDevice, const Mango::LogicalDevice& logicalDevice)
	: _logicalDevice(logicalDevice), _physicalDevice(physicalDevice)
{
}

uint64_t Mango::UniformBuffersPool::CreateBuffer(const UniformBufferCreateInfo createInfo)
{
	const auto& bufferId = createInfo.BufferId;
	M_ASSERT(!_uniformBuffers.contains(bufferId) && "Specified buffer id already exist");

	_uniformBuffers[bufferId] = std::make_unique<Mango::UniformBuffer>(
		createInfo.BufferSize,
		_physicalDevice,
		_logicalDevice
	);

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = _uniformBuffers.at(bufferId)->GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = createInfo.BufferSize;

	VkWriteDescriptorSet writeDescriptorInfo{};
	writeDescriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorInfo.dstSet = createInfo.DestinationSet;
	writeDescriptorInfo.dstBinding = createInfo.Binding;
	writeDescriptorInfo.dstArrayElement = 0; // NOTE: This may be used in future and parameterized
	writeDescriptorInfo.descriptorType = createInfo.DescriptorType;
	writeDescriptorInfo.descriptorCount = 1;
	writeDescriptorInfo.pBufferInfo = &bufferInfo;
	writeDescriptorInfo.pImageInfo = nullptr; // NOTE: This may be used in future and parameterized
	writeDescriptorInfo.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(_logicalDevice.GetDevice(), 1, &writeDescriptorInfo, 0, nullptr);
	return bufferId;
}

void Mango::UniformBuffersPool::UpdateDescriptorSet(Mango::UniformBuffer& buffer, VkDescriptorSet destinationSet, uint32_t binding, VkDescriptorType descriptorType)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = buffer.GetSize();

	VkWriteDescriptorSet writeDescriptorInfo{};
	writeDescriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorInfo.dstSet = destinationSet;
	writeDescriptorInfo.dstBinding = binding;
	writeDescriptorInfo.dstArrayElement = 0; // NOTE: This may be used in future and parameterized
	writeDescriptorInfo.descriptorType = descriptorType;
	writeDescriptorInfo.descriptorCount = 1;
	writeDescriptorInfo.pBufferInfo = &bufferInfo;
	writeDescriptorInfo.pImageInfo = nullptr; // NOTE: This may be used in future and parameterized
	writeDescriptorInfo.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(_logicalDevice.GetDevice(), 1, &writeDescriptorInfo, 0, nullptr);
}
