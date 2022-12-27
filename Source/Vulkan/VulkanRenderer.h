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
#include "UniformBuffersPool.h"
#include "UniformBufferObject.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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
		void Draw(Mango::VertexBuffer& vertexBuffer, Mango::IndexBuffer& indexBuffer); // TODO: Temporary
		void HandleFramebuffersResized() override;

		// TODO: Temporary, remove this
		void UpdateUniformBuffer(uint32_t currentFrame);

	private:
		bool _isOffscreen;
		std::unique_ptr<Mango::RenderPass> _renderPass;
		Mango::RenderPassCreateInfo _renderPassCreateInfo;

		Mango::HardwareInfo _hardwareInfo;
		Mango::RenderSurface& _renderSurface;
		Mango::LogicalDevice& _logicalDevice;
		Mango::SwapChain& _swapChain;
		uint32_t _maxFramesInFlight;

		std::unique_ptr<Mango::DescriptorSetLayout> _globalDescriptorSetLayout;
		//std::unique_ptr<Mango::DescriptorSetLayout> _textureDescriptorSetLayout;
		//std::unique_ptr<Mango::DescriptorSetLayout> _localDescriptorSetLayout;

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

		uint32_t _currentFrame = 0;

		// Game objects (temporary)
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
