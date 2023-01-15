#include "FramebuffersPool.h"

Mango::FramebuffersPool::FramebuffersPool(
	const Mango::LogicalDevice& logicalDevice,
	const Mango::RenderPass& renderPass,
	const Mango::RenderArea& renderArea,
	const Mango::RenderAreaInfo& renderAreaInfo
) : _framebuffersCount(static_cast<uint32_t>(renderAreaInfo.ImageViews.size()))
{
	VkExtent2D extent{};
	extent.width = renderArea.Width;
	extent.height = renderArea.Height;
	const auto& imageViews = renderAreaInfo.ImageViews;
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
