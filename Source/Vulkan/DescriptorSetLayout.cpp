#include "DescriptorSetLayout.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::DescriptorSetLayout::DescriptorSetLayout(Mango::LogicalDevice& logicalDevice, std::vector<VkDescriptorSetLayoutBinding> bindings)
	: _logicalDevice(logicalDevice.GetDevice())
{
	_descriptorSetId = static_cast<uint64_t>(std::rand()); // TODO: std::rand() is not a good solution for this, it may produce hard to debug collisions
	const uint32_t bindingsCount = static_cast<uint32_t>(bindings.size());
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindingsCount;
	layoutInfo.pBindings = bindings.data();

	auto createDescriptorSetLayoutResult = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, nullptr, &_layout);
	M_TRACE("Create descriptor set layout result is: " + std::to_string(createDescriptorSetLayoutResult));
	M_ASSERT(createDescriptorSetLayoutResult == VK_SUCCESS && "Failed to create descriptor set layout");
}

Mango::DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(_logicalDevice, _layout, nullptr);
}

Mango::DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(Mango::LogicalDevice& logicalDevice) : _logicalDevice(logicalDevice)
{
}

Mango::DescriptorSetLayoutBuilder& Mango::DescriptorSetLayoutBuilder::AddBinding(
	uint64_t binding,
	VkDescriptorType descriptorType,
	VkShaderStageFlags shaderStages
)
{
	M_ASSERT(!_bindings.contains(binding) && "Specified binding already exists in descriptor set layout");

	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = shaderStages;
	
	_bindings[binding] = layoutBinding;
	return *this;
}

std::unique_ptr<Mango::DescriptorSetLayout> Mango::DescriptorSetLayoutBuilder::Build()
{
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	for (const auto& [_, binding] : _bindings)
	{
		descriptorSetLayoutBindings.push_back(binding);
	}
	return std::make_unique<Mango::DescriptorSetLayout>(_logicalDevice, descriptorSetLayoutBindings);
}
