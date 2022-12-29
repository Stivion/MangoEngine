#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "CommandBuffer.h"
#include "CommandBufferRecorder.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	struct UICommandBufferRecorderInfo
	{
		const Mango::PhysicalDevice* PhysicalDevice;
		const Mango::LogicalDevice* LogicalDevice;
		const Mango::GraphicsPipeline* GraphicsPipeline;
		const Mango::SwapChain* SwapChain;
		VkImage ViewportImage;
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
		VkImageMemoryBarrier CreateImageMemoryBarrier(
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask
		);

	private:
		const Mango::PhysicalDevice* _physicalDevice;
		const Mango::LogicalDevice* _logicalDevice;
		const Mango::GraphicsPipeline* _graphicsPipeline;
		const Mango::SwapChain* _swapChain;
		const VkImage _viewportImage;
		const Mango::ViewportInfo _viewportInfo;

		VkImageMemoryBarrier _firstTransitionBarrier;
		VkImageMemoryBarrier _secondTransitionBarrier;
	};
}
