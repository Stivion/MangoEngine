#pragma once

#include "RenderPass.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class CommandBuffer
	{
	public:
		CommandBuffer() = delete;

		void BeginCommandBuffer(const VkFramebuffer& framebuffer);
		void DrawIndexed(
			const Mango::GraphicsPipeline& graphicsPipeline,
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
			Mango::SwapChain& swapChain
		);

	private:
		VkCommandBuffer _commandBuffer;
		Mango::RenderPass& _renderPass;
		Mango::SwapChain& _swapChain;

		friend class CommandBuffersPool;
	};
}