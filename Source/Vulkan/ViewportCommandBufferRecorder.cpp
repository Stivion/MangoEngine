#include "ViewportCommandBufferRecorder.h"

Mango::ViewportCommandBufferRecorder::ViewportCommandBufferRecorder(ViewportCommandBufferRecorderInfo recorderInfo)
	: _descriptorSets(recorderInfo.DescriptorSets), _viewportInfo(recorderInfo.ViewportInfo)
{
	_graphicsPipeline = recorderInfo.GraphicsPipeline;
	_descriptorPool = recorderInfo.DescriptorPool;
	_vertexBuffer = recorderInfo.VertexBuffer;
	_indexBuffer = recorderInfo.IndexBuffer;
}

const void Mango::ViewportCommandBufferRecorder::RecordCommandBuffer(
	const Mango::CommandBuffer* commandBuffer,
	const Mango::Framebuffer* framebuffer
) const
{
	commandBuffer->Reset();
	commandBuffer->BeginCommandBuffer();
	commandBuffer->BeginRenderPass(
		framebuffer->GetSwapChainFramebuffer(),
		_graphicsPipeline->GetGraphicsPipeline(),
		_viewportInfo
	);

	commandBuffer->DrawIndexed(*_vertexBuffer, *_indexBuffer, _descriptorSets);

	commandBuffer->EndRenderPass();
	commandBuffer->EndCommandBuffer();
}
