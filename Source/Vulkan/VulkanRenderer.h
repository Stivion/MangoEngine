#pragma once

#include "../Render/Renderer.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"
#include "RenderSurface.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "SwapChainSupportDetails.h"
#include "RenderPass.h"
#include "FramebuffersPool.h"
#include "CommandPool.h"
#include "CommandBuffersPool.h"
#include "Fence.h"
#include "Semaphore.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"
#include "UniformBuffersPool.h"
#include "UniformBufferObject.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ViewportCommandBufferRecorder.h"
#include "UICommandBufferRecorder.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace Mango
{
	struct VulkanRendererCreateInfo
	{
		uint32_t MaxFramesInFlight;
		uint32_t WindowFramebufferWidth;
		uint32_t WindowFramebufferHeight;
		Mango::Instance* Instance;
		Mango::RenderSurface* RenderSurface;
		Mango::PhysicalDevice* PhysicalDevice;
		Mango::QueueFamilyIndices* QueueFamilyIndices;
		Mango::LogicalDevice* LogicalDevice;
	};

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(const VulkanRendererCreateInfo createInfo);
		VulkanRenderer() = delete;
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer operator=(const VulkanRenderer&) = delete;
		~VulkanRenderer() = default;

		void Draw() override;
		void Draw(Mango::VertexBuffer& vertexBuffer, Mango::IndexBuffer& indexBuffer); // TODO: Temporary
		void HandleFramebuffersResized() override;

		// TODO: Temporary, remove this
		void UpdateUniformBuffer(uint32_t currentFrame);

	private:
		// This members is not owned by renderer
		uint32_t _maxFramesInFlight;
		const Mango::Instance* _instance;
		const Mango::PhysicalDevice* _physicalDevice;
		const Mango::QueueFamilyIndices* _queueFamilyIndices;
		const Mango::RenderSurface* _renderSurface;
		const Mango::LogicalDevice* _logicalDevice;

	private:
		std::unique_ptr<Mango::SwapChain> _swapChain;
		std::unique_ptr<Mango::RenderPass> _renderPass;
		std::unique_ptr<Mango::DescriptorSetLayout> _globalDescriptorSetLayout;
		const std::vector<VkDescriptorPoolSize> _poolSizes = // TODO: Figure out how to allocate correct pool sizes
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 2 }
		};
		std::unique_ptr<Mango::DescriptorPool> _descriptorPool;
		std::unique_ptr<Mango::GraphicsPipeline> _graphicsPipeline;
		std::unique_ptr<Mango::UniformBuffersPool> _uniformBuffers;
		std::unique_ptr<Mango::FramebuffersPool> _framebuffers;
		std::unique_ptr<Mango::CommandPool> _commandPool;
		std::unique_ptr<Mango::CommandBuffersPool> _commandBuffers;
		std::vector<std::unique_ptr<Mango::Fence>> _fences;
		std::vector<std::unique_ptr<Mango::Semaphore>> _imageAvailableSemaphores;
		std::vector<std::unique_ptr<Mango::Semaphore>> _renderFinishedSemaphores;

		std::unique_ptr<Mango::ViewportCommandBufferRecorder> _viewportCommandBufferRecorder;
		std::unique_ptr<Mango::UICommandBufferRecorder> _uiCommandBufferRecorder;

		uint32_t _currentFrame = 0;

		// TODO: Game objects (temporary)
		const std::vector<Mango::Vertex> _vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},

			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{1.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}}
		};
		const std::vector<uint16_t> _indices =
		{
			0, 1, 2, 3, 4, 5, 5, 6, 3
		};
		std::unique_ptr<Mango::VertexBuffer> _vertexBuffer;
		std::unique_ptr<Mango::IndexBuffer> _indexBuffer;
	};
}
