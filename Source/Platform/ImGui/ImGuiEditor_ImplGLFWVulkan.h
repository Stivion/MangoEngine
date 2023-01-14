#pragma once

#include "../../ImGui/ImGuiEditor.h"

#include "../Vulkan/Context.h"
#include "../Vulkan/SwapChain.h"
#include "../Vulkan/SwapChainSupportDetails.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/FramebuffersPool.h"
#include "../Vulkan/DescriptorPool.h"
#include "../Vulkan/CommandPool.h"
#include "../Vulkan/CommandBuffersPool.h"

#include "../Windowing/GLFWWindow.h"
#include "ImGuiEditorViewport_ImplVulkan.h"

#include "vulkan/vulkan.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "vector"
#include "memory"

namespace Mango
{
	struct ImGuiEditor_ImplGLFWVulkan_CreateInfo
	{
		uint32_t MaxFramesInFlight;
		const Mango::Context* VulkanContext;
		Mango::RenderArea RenderArea;
		Mango::RenderArea ViewportRenderArea;
		Mango::RenderAreaInfo RenderAreaInfo;
		Mango::RenderAreaInfo ViewportAreaInfo;
	};

	class ImGuiEditor_ImplGLFWVulkan : public ImGuiEditor
	{
	public:
		ImGuiEditor_ImplGLFWVulkan(ImGuiEditor_ImplGLFWVulkan_CreateInfo createInfo);
		ImGuiEditor_ImplGLFWVulkan(const ImGuiEditor_ImplGLFWVulkan&) = delete;
		ImGuiEditor_ImplGLFWVulkan operator=(const ImGuiEditor_ImplGLFWVulkan&) = delete;
		~ImGuiEditor_ImplGLFWVulkan();

		void NewFrame(uint32_t currentFrame) override;
		void EndFrame() override;
		const Mango::CommandBuffer& RecordCommandBuffer(uint32_t imageIndex);

		void HandleResize(Mango::RenderArea renderArea, Mango::RenderAreaInfo renderAreaInfo);
		void HandleViewportResize(const Mango::RenderArea viewportRenderArea, const Mango::RenderAreaInfo viewportRenderAreaInfo);

		const Mango::CommandBuffer& GetCurrentCommandBuffer() const { return _imGuiCommandBuffers->GetCommandBuffer(_currentFrame); }

	private:
		static void CheckImGuiVulkanResultFn(VkResult result);
		VkImageMemoryBarrier CreateImageMemoryBarrier(
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask
		);

	private:
		const uint32_t _maxFramesInFlight;
		const Mango::Context* _vulkanContext;
		Mango::RenderArea _renderArea;
		Mango::RenderArea _viewportRenderArea;
		Mango::RenderAreaInfo _renderAreaInfo;
		Mango::RenderAreaInfo _viewportRenderAreaInfo;

		const std::vector<VkDescriptorPoolSize> _imGuiPoolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		std::unique_ptr<Mango::RenderPass> _imGuiRenderPass;
		std::unique_ptr<Mango::FramebuffersPool> _imGuiFramebuffers;
		std::unique_ptr<Mango::DescriptorPool> _imGuiDescriptorPool;
		std::unique_ptr<Mango::CommandPool> _imGuiCommandPool;
		std::unique_ptr<Mango::CommandBuffersPool> _imGuiCommandBuffers;
		std::unique_ptr<Mango::ImGuiEditorViewport_ImplVulkan> _imGuiEditorViewport;
		uint32_t _currentFrame;

		VkImageMemoryBarrier _toDestinationTransitionBarrier;
		VkImageMemoryBarrier _toShaderReadTransitionBarrier;
		VkImageSubresourceLayers _imageSubresourceLayers;
		VkImageCopy _copyRegion;
	};
}
