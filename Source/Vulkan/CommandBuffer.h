#pragma once

#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	struct ViewportInfo
	{
		float X, Y, Width, Height, MinDepth, MaxDepth;
	};

	class CommandBuffer
	{
	public:
		CommandBuffer() = delete;

		void BeginCommandBuffer() const;
		void BeginRenderPass(const VkFramebuffer& framebuffer, const VkPipeline& graphicsPipeline, const ViewportInfo viewportInfo) const;
		void DrawIndexed(
			const Mango::VertexBuffer& vertexBuffer,
			const Mango::IndexBuffer& indexBuffer,
			std::vector<VkDescriptorSet> descriptors
		) const;
		void EndRenderPass() const;
		void EndCommandBuffer() const;
		void Reset() const;

		const VkCommandBuffer& GetVkCommandBuffer() const { return _commandBuffer; }

	private:
		CommandBuffer(
			VkCommandBuffer commandBuffer,
			const Mango::RenderPass& renderPass,
			const Mango::SwapChain& swapChain,
			const Mango::GraphicsPipeline& graphicsPipeline
		);

	private:
		VkCommandBuffer _commandBuffer;
		const Mango::RenderPass& _renderPass;
		const Mango::SwapChain& _swapChain;
		const Mango::GraphicsPipeline& _graphicsPipeline;

		friend class CommandBuffersPool;
	};
}