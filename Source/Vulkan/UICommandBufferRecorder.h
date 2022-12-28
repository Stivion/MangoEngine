#pragma once

#include "CommandBuffer.h"
#include "CommandBufferRecorder.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"

namespace Mango
{
	struct UICommandBufferRecorderInfo
	{
		Mango::GraphicsPipeline* GraphicsPipeline;
		Mango::ViewportInfo ViewportInfo;
	};

	class UICommandBufferRecorder : public CommandBufferRecorder
	{
	public:
		UICommandBufferRecorder(UICommandBufferRecorderInfo recorderInfo);
		const void RecordCommandBuffer(
			const Mango::CommandBuffer* commandBuffer,
			const Mango::Framebuffer* framebuffer
		) const override;

	private:
		const Mango::GraphicsPipeline* _graphicsPipeline;
		const Mango::ViewportInfo _viewportInfo;

	};
}
