#pragma once

#include "../Vulkan/PhysicalDevice.h"
#include "../Vulkan/LogicalDevice.h"
#include "../Vulkan/RenderArea.h"

#include <vulkan/vulkan.h>
#include <imgui_impl_vulkan.h>

namespace Mango
{
	struct ImGuiEditorViewport_ImplVulkan_CreateInfo
	{
		const Mango::PhysicalDevice* PhysicalDevice;
		const Mango::LogicalDevice* LogicalDevice;
		const Mango::RenderArea* RenderArea; // TODO: Passing this as pointer is incorrect
	};

	class ImGuiEditorViewport_ImplVulkan
	{
	public:
		ImGuiEditorViewport_ImplVulkan(ImGuiEditorViewport_ImplVulkan_CreateInfo createInfo);
		ImGuiEditorViewport_ImplVulkan(const ImGuiEditorViewport_ImplVulkan&) = delete;
		ImGuiEditorViewport_ImplVulkan operator=(const ImGuiEditorViewport_ImplVulkan&) = delete;
		~ImGuiEditorViewport_ImplVulkan();

		void RecreateEditorViewport(const Mango::RenderArea* renderArea);

		const VkImage& GetViewportImage() const { return _viewportImage; }
		const VkDescriptorSet& GetViewportImageDescriptorSet() const { return _viewportImageDescriptorSet; }

	private:
		void InitializeViewportImage();
		void InitializeViewportImageView();
		void InitializeImageSampler();
		void DestroyEditorViewport();

	private:
		VkImage _viewportImage;
		VkDeviceMemory _viewportImageMemory;
		VkImageView _viewportImageView;
		VkSampler _viewportImageSampler;
		VkDescriptorSet _viewportImageDescriptorSet;

		const Mango::PhysicalDevice* _physicalDevice;
		const Mango::LogicalDevice* _logicalDevice;
		const Mango::RenderArea* _renderArea;
	};
}
