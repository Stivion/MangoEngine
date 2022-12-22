#include "FramebuffersPool.h"

Mango::FramebuffersPool::FramebuffersPool(
	Mango::LogicalDevice& logicalDevice,
	Mango::RenderPass& renderPass,
	Mango::SwapChain& swapChain
) : _framebuffersCount(static_cast<uint32_t>(swapChain.GetSwapChainImageViews().size()))
{
	const auto& extent = swapChain.GetSwapChainExtent();
	const auto& imageViews = swapChain.GetSwapChainImageViews();
	_framebuffers.resize(_framebuffersCount);

	for (size_t i = 0; i < imageViews.size(); i++)
	{
		Mango::Framebuffer* framebuffer = new Mango::Framebuffer(logicalDevice, renderPass, extent, imageViews[i]);
		_framebuffers[i] = framebuffer;
	}
}

Mango::FramebuffersPool::~FramebuffersPool()
{
	for (const auto framebufffer : _framebuffers)
	{
		delete framebufffer;
	}
}
