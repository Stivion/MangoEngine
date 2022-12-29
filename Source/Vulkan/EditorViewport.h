#pragma once

#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "SwapChain.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	struct EditorViewportCreateInfo
	{
		const Mango::PhysicalDevice* PhysicalDevice;
		const Mango::LogicalDevice* LogicalDevice;
		const Mango::SwapChain* SwapChain;
	};

	class EditorViewport
	{
	public:
		EditorViewport(EditorViewportCreateInfo createInfo);
		EditorViewport(const EditorViewport&) = delete;
		EditorViewport operator=(const EditorViewport&) = delete;
		~EditorViewport();

		const VkImage& GetViewportImage() const { return _viewportImage; }
		const VkDescriptorSet& GetViewportImageDescriptorSet() const { return _viewportImageDescriptorSet; }

	private:
		void InitializeViewportImage();
		void InitializeViewportImageView();
		void InitializeImageSampler();

		VkImage _viewportImage;
		VkDeviceMemory _viewportImageMemory;
		VkImageView _viewportImageView;
		VkSampler _viewportImageSampler;
		VkDescriptorSet _viewportImageDescriptorSet;

		const Mango::PhysicalDevice* _physicalDevice;
		const Mango::LogicalDevice* _logicalDevice;
		const Mango::SwapChain* _swapChain;
	};
}