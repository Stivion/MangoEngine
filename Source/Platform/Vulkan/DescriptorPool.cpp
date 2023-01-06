#include "DescriptorPool.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorPool::DescriptorPool(
	const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	const Mango::LogicalDevice& logicalDevice,
	const VkDescriptorPoolCreateFlags flags
) : _logicalDevice(logicalDevice.GetDevice())
{
	uint32_t maxSets = 0;
	for (const auto& poolSize : descriptorPoolSizes)
	{
		maxSets += poolSize.descriptorCount;
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	poolInfo.pPoolSizes = descriptorPoolSizes.data();
	poolInfo.maxSets = maxSets; // NOTE: Maybe we need some sophisticateg logic here
	poolInfo.flags = flags;

	auto createDescriptorPoolResult = vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool);
	M_TRACE("Create descriptor pool resuls is: " + std::to_string(createDescriptorPoolResult));
	M_ASSERT(createDescriptorPoolResult == VK_SUCCESS && "Failed to create descriptor pool");
}

Mango::DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, nullptr);
}

void Mango::DescriptorPool::AllocateDescriptorSets(std::vector<const Mango::DescriptorSetLayout*> descriptorSetLayouts)
{
	const uint32_t setCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	std::vector<VkDescriptorSetLayout> setLayouts(setCount);
	for (size_t i = 0; i < setCount; i++)
	{
		setLayouts[i] = descriptorSetLayouts[i]->GetDescriptorSetLayout();
	}

	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = _descriptorPool;
	allocateInfo.descriptorSetCount = setCount;
	allocateInfo.pSetLayouts = setLayouts.data();

	std::vector<VkDescriptorSet> descriptorSets(setCount);
	auto allocateDescriptorSetsResult = vkAllocateDescriptorSets(_logicalDevice, &allocateInfo, descriptorSets.data());
	M_TRACE("Allocate descriptor sets result is: " + std::to_string(allocateDescriptorSetsResult));
	M_ASSERT(allocateDescriptorSetsResult == VK_SUCCESS && "Failed to allocate descriptor sets");

	for (size_t i = 0; i < setCount; i++)
	{
		_descriptorSets[descriptorSetLayouts[i]->GetId()] = descriptorSets[i];
	}
}
