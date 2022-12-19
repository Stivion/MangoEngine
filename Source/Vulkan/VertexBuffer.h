#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class VertexBuffer
	{
	public:
		VertexBuffer(
			uint32_t vertexCount,
			uint32_t bufferSizeBytes,
			const void* memory,
			Mango::PhysicalDevice& physicalDevice,
			Mango::LogicalDevice& logicalDevice
		);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer operator=(const VertexBuffer&) = delete;
		~VertexBuffer();

		const VkBuffer& GetBuffer() const { return _buffer; }
		const uint32_t GetVertexCount() const { return _vertexCount; }

	private:
		VkBuffer _buffer;
		uint32_t _vertexCount;
		VkDeviceMemory _vertexBufferMemory;
		VkDevice& _logicalDevice;

	private:
		void AllocateMemory(Mango::PhysicalDevice& physicalDevice);
		void MapMemory(uint32_t bufferSizeBytes, const void* memoryBuffer);
	};
}