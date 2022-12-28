#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "CommandPool.h"
#include "Buffer.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(
			uint32_t vertexCount,
			VkDeviceSize bufferSizeBytes,
			const void* memory,
			const Mango::PhysicalDevice& physicalDevice,
			const Mango::LogicalDevice& logicalDevice,
			const Mango::CommandPool& commandPool
		);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer operator=(const VertexBuffer&) = delete;

		const uint32_t GetVertexCount() const { return _vertexCount; }

	private:
		uint32_t _vertexCount;

	private:
		void CopyToVertexBuffer(
			const VkBuffer& sourceBuffer,
			const VkCommandPool& commandPool,
			const VkQueue& graphicsQueue,
			uint32_t bufferSizeBytes
		);
	};
}