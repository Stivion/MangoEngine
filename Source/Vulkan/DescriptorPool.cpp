#include "DescriptorPool.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorPool::DescriptorPool(
	const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	Mango::LogicalDevice& logicalDevice
) : _logicalDevice(logicalDevice.GetDevice())
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
	poolInfo.pPoolSizes = descriptorPoolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(descriptorPoolSizes.size());

	auto createDescriptorPoolResult = vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool);
	M_TRACE("Create descriptor pool resuls is: " + std::to_string(createDescriptorPoolResult));
	M_ASSERT(createDescriptorPoolResult == VK_SUCCESS && "Failed to create descriptor pool");
}

Mango::DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, nullptr);
}
