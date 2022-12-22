#pragma once

#include "RenderPass.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class CommandBuffer
	{
	public:
		CommandBuffer() = delete;
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer operator=(const CommandBuffer&) = delete;

		void BeginCommandBuffer(const VkFramebuffer& framebuffer);
		void DrawIndexed(
			const Mango::VertexBuffer& vertexBuffer,
			const Mango::IndexBuffer& indexBuffer,
			const VkDescriptorSet& descriptorSet
		);
		void EndCommandBuffer();

		const VkCommandBuffer& GetVkCommandBuffer() const { return _commandBuffer; }

	private:
		CommandBuffer(
			VkCommandBuffer commandBuffer,
			Mango::RenderPass& renderPass,
			Mango::SwapChain& swapChain,
			Mango::GraphicsPipeline& graphicsPipeline
		);

	private:
		VkCommandBuffer _commandBuffer;
		Mango::RenderPass& _renderPass;
		Mango::SwapChain& _swapChain;
		Mango::GraphicsPipeline& _graphicsPipeline;

		friend class CommandBuffersPool;
	};
}