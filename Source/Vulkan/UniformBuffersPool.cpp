#include "UniformBuffersPool.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::UniformBuffersPool::UniformBuffersPool(Mango::PhysicalDevice& physicalDevice, Mango::LogicalDevice& logicalDevice)
	: _logicalDevice(logicalDevice), _physicalDevice(physicalDevice)
{
}

uint64_t Mango::UniformBuffersPool::CreateBuffer(const UniformBufferCreateInfo createInfo)
{
	const auto& bufferId = createInfo.BufferId;
	M_ASSERT(!_uniformBuffers.contains(bufferId) && "Specified buffer id already exist");

	_uniformBuffers[bufferId] = std::make_unique<const Mango::UniformBuffer>(
		createInfo.BufferSize,
		_physicalDevice,
		_logicalDevice
	);

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = _uniformBuffers.at(bufferId)->GetBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet writeDescriptorInfo{};
	writeDescriptorInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorInfo.dstSet = createInfo.DestinationSet;
	writeDescriptorInfo.dstBinding = createInfo.Binding;
	writeDescriptorInfo.dstArrayElement = 0; // This may be used in future and parameterized
	writeDescriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorInfo.descriptorCount = 1;
	writeDescriptorInfo.pBufferInfo = &bufferInfo;
	writeDescriptorInfo.pImageInfo = nullptr; // This may be used in future and parameterized
	writeDescriptorInfo.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(_logicalDevice.GetDevice(), 1, &writeDescriptorInfo, 0, nullptr);
	return bufferId;
}
