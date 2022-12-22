#pragma once

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class CommandBuffersPool
	{
	public:
		CommandBuffersPool(
			uint32_t commandBuffersCount,
			Mango::LogicalDevice& logicalDevice,
			Mango::SwapChain& swapChain,
			Mango::RenderPass& renderPass,
			Mango::GraphicsPipeline& graphicsPipeline,
			Mango::CommandPool& commandPool
		);
		CommandBuffersPool() = delete;
		CommandBuffersPool(const CommandBuffersPool&) = delete;
		CommandBuffersPool operator=(const CommandBuffersPool&) = delete;
		~CommandBuffersPool();

		Mango::CommandBuffer& GetCommandBuffer(uint32_t index) { return *_commandBuffers[index]; }

	private:
		std::vector<Mango::CommandBuffer*> _commandBuffers;
	};
}
