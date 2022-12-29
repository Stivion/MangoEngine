#pragma once

#include "CommandBuffer.h"
#include "CommandBufferRecorder.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

// TODO: Temporary
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace Mango
{
	struct ViewportCommandBufferRecorderInfo
	{
		Mango::GraphicsPipeline* GraphicsPipeline;
		Mango::DescriptorPool* DescriptorPool;
		std::vector<VkDescriptorSet> DescriptorSets;
		Mango::ViewportInfo ViewportInfo;

		// TODO: Temporary
		Mango::VertexBuffer* VertexBuffer;
		Mango::IndexBuffer* IndexBuffer;
	};

	class ViewportCommandBufferRecorder : public CommandBufferRecorder
	{
	public:
		ViewportCommandBufferRecorder(ViewportCommandBufferRecorderInfo recorderInfo);
		const void RecordCommandBuffer(
			const Mango::CommandBuffer* commandBuffer,
			const Mango::Framebuffer* framebuffer
		) const override;

	private:
		const Mango::GraphicsPipeline* _graphicsPipeline;
		const Mango::DescriptorPool* _descriptorPool;
		std::vector<VkDescriptorSet> _descriptorSets;
		const Mango::ViewportInfo _viewportInfo;
		
		// TODO: Temporary
		Mango::VertexBuffer* _vertexBuffer;
		Mango::IndexBuffer* _indexBuffer;
	};
}
