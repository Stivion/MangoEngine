#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Buffer.h"
#include "CommandPool.h"

namespace Mango
{
	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer(
			uint32_t indicesCount,
			VkDeviceSize bufferSizeBytes,
			const void* memory,
			const Mango::PhysicalDevice& physicalDevice,
			const Mango::LogicalDevice& logicalDevice,
			const Mango::CommandPool& commandPool
		);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer operator=(const IndexBuffer&) = delete;

		const uint32_t GetIndicesCount() const { return _indicesCount; }

	private:
		uint32_t _indicesCount;

	private:
		void CopyToIndexBuffer(
			const VkBuffer& sourceBuffer,
			const VkCommandPool& commandPool,
			const VkQueue& graphicsQueue,
			uint32_t bufferSizeBytes
		);
	};
}