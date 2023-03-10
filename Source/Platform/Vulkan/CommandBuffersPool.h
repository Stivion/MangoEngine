#pragma once

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "RenderArea.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class CommandBuffersPool
	{
	public:
		CommandBuffersPool(
			uint32_t commandBuffersCount,
			const Mango::LogicalDevice& logicalDevice,
			const Mango::RenderPass& renderPass,
			const Mango::CommandPool& commandPool,
			const Mango::RenderArea& renderArea
		);
		CommandBuffersPool() = delete;
		CommandBuffersPool(const CommandBuffersPool&) = delete;
		CommandBuffersPool operator=(const CommandBuffersPool&) = delete;
		~CommandBuffersPool();

		const Mango::CommandBuffer& GetCommandBuffer(uint32_t index) const { return *_commandBuffers[index]; }

	private:
		std::vector<Mango::CommandBuffer*> _commandBuffers;
	};
}
