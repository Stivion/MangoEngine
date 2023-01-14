#pragma once

#include "../Vulkan/PhysicalDevice.h"
#include "../Vulkan/LogicalDevice.h"
#include "../Vulkan/RenderArea.h"
#include "../Vulkan/Image.h"

#include <vulkan/vulkan.h>
#include <imgui_impl_vulkan.h>

#include <memory>

namespace Mango
{
	struct ImGuiEditorViewport_ImplVulkan_CreateInfo
	{
		const Mango::Context* VulkanContext;
		Mango::RenderArea RenderArea;
		Mango::RenderAreaInfo RenderAreaInfo;
	};

	class ImGuiEditorViewport_ImplVulkan
	{
	public:
		ImGuiEditorViewport_ImplVulkan(ImGuiEditorViewport_ImplVulkan_CreateInfo createInfo);
		ImGuiEditorViewport_ImplVulkan(const ImGuiEditorViewport_ImplVulkan&) = delete;
		ImGuiEditorViewport_ImplVulkan operator=(const ImGuiEditorViewport_ImplVulkan&) = delete;
		~ImGuiEditorViewport_ImplVulkan();

		void RecreateEditorViewport(const Mango::RenderArea renderArea, const Mango::RenderAreaInfo renderAreaInfo);

		const VkImage& GetViewportImage() const { return _image->Get(); }
		const VkDescriptorSet& GetViewportImageDescriptorSet() const { return _viewportImageDescriptorSet; }

	private:
		void InitializeImageSampler();
		void DestroyEditorViewport();

	private:
		std::unique_ptr<Mango::Image> _image;
		VkSampler _viewportImageSampler;
		VkDescriptorSet _viewportImageDescriptorSet;

		const Mango::Context* _vulkanContext;
		Mango::RenderArea _renderArea;
		Mango::RenderAreaInfo _renderAreaInfo;
	};
}
