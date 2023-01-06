#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "UniformBuffer.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <unordered_map>
#include <memory>

namespace Mango
{
	struct UniformBufferCreateInfo
	{
		uint64_t BufferId; // User-defined, used for tracking buffers. Must be unique. (May be removed later for UUID)
		VkDeviceSize BufferSize; // Size of the actual type for this buffer.
		VkDescriptorSet DestinationSet; // Descriptor set for new buffer.
		uint32_t Binding; // Binding of uniform object.
	};

	class UniformBuffersPool
	{
	public:
		UniformBuffersPool(const Mango::PhysicalDevice& physicalDevice, const Mango::LogicalDevice& logicalDevice);
		UniformBuffersPool() = delete;
		UniformBuffersPool(const UniformBuffersPool&) = delete;
		UniformBuffersPool operator=(const UniformBuffersPool&) = delete;
		~UniformBuffersPool() = default;

		uint64_t CreateBuffer(const UniformBufferCreateInfo createInfo);

		const Mango::UniformBuffer& GetUniformBuffer(uint64_t bufferId) const { return *_uniformBuffers.at(bufferId); }

	private:
		// Uniform buffers is owned by this class
		std::unordered_map<uint64_t, std::unique_ptr<const Mango::UniformBuffer>> _uniformBuffers;
		const Mango::LogicalDevice& _logicalDevice;
		const Mango::PhysicalDevice& _physicalDevice;
	};
}
