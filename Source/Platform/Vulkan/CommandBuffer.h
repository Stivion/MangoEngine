#pragma once

#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "Framebuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderArea.h"

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
		void BeginRenderPass(const VkFramebuffer& framebuffer, const Mango::RenderArea renderArea) const;
		void DrawIndexed(
			const Mango::VertexBuffer& vertexBuffer,
			const Mango::IndexBuffer& indexBuffer,
			std::vector<uint32_t>& indicesPerDraw,
			std::vector<uint32_t>& dynamicOffsets,
			std::vector<VkDescriptorSet> descriptors,
			const VkPipelineLayout& pipelineLayout
		) const;
		void EndRenderPass() const;
		void EndCommandBuffer() const;
		void Reset() const;
		void BindPipeline(const Mango::GraphicsPipeline& pipeline) const;

		const VkCommandBuffer& GetVkCommandBuffer() const { return _commandBuffer; }

	private:
		CommandBuffer(VkCommandBuffer commandBuffer, const Mango::RenderPass& renderPass);

	private:
		VkCommandBuffer _commandBuffer;
		const Mango::RenderPass& _renderPass;

		friend class CommandBuffersPool;
	};
}