#pragma once

#include "../Render/Renderer.h"

#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderSurface.h"
#include "RenderPass.h"
#include "FramebuffersPool.h"
#include "CommandPool.h"
#include "CommandBuffersPool.h"
#include "Fence.h"
#include "Semaphore.h"
#include "HardwareInfo.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace Mango
{
	struct VulkanRendererCreateInfo
	{
		uint32_t MaxFramesInFlight;
		// TODO: Make them smart (shared?) pointers
		Mango::HardwareInfo HardwareInfo;
		Mango::RenderSurface& RenderSurface;
		Mango::LogicalDevice& LogicalDevice;
		Mango::SwapChain& SwapChain;
	};

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(bool isOffscreen, const VulkanRendererCreateInfo createInfo);
		VulkanRenderer() = delete;
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer operator=(const VulkanRenderer&) = delete;
		~VulkanRenderer();

		void Draw() override;
		void HandleFramebuffersResized() override;

	private:
		std::unique_ptr<Mango::RenderPass> _renderPass;
		Mango::RenderPassCreateInfo _renderPassCreateInfo;

		Mango::HardwareInfo _hardwareInfo;
		Mango::RenderSurface& _renderSurface;
		Mango::LogicalDevice& _logicalDevice;
		Mango::SwapChain& _swapChain;
		uint32_t _maxFramesInFlight;

		const std::vector<VkDescriptorPoolSize> _poolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 2 }
		};
		std::unique_ptr<Mango::DescriptorSetLayout> _descriptorSetLayout;
		std::unique_ptr<Mango::DescriptorPool> _descriptorPool;
		std::unique_ptr<Mango::GraphicsPipeline> _graphicsPipeline;
		std::unique_ptr<Mango::FramebuffersPool> _framebuffers;
		std::unique_ptr<Mango::CommandPool> _commandPool;
		std::unique_ptr<Mango::CommandBuffersPool> _commandBuffers;
		std::vector<std::shared_ptr<Mango::Fence>> _fences;
		std::vector<std::shared_ptr<Mango::Semaphore>> _imageAvailableSemaphores;
		std::vector<std::shared_ptr<Mango::Semaphore>> _renderFinishedSemaphores;

		uint32_t _currentFrame = 0;
	};
}
