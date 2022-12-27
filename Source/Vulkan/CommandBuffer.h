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

		void BeginCommandBuffer();
		void BeginRenderPass(const VkFramebuffer& framebuffer, const VkPipeline& graphicsPipeline, const ViewportInfo viewportInfo);
		void DrawIndexed(const Mango::VertexBuffer& vertexBuffer, const Mango::IndexBuffer& indexBuffer, std::vector<VkDescriptorSet>& descriptors);
		void EndRenderPass();
		void EndCommandBuffer();
		void Reset();

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