#pragma once

#include "../../Render/Renderer.h"

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

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace Mango
{
	struct Renderer_ImplVulkan_CreateInfo
	{
		uint32_t MaxFramesInFlight;
		uint32_t WindowFramebufferWidth;
		uint32_t WindowFramebufferHeight;
		const Mango::Instance* Instance;
		const Mango::RenderSurface* RenderSurface;
		const Mango::PhysicalDevice* PhysicalDevice;
		const Mango::QueueFamilyIndices* QueueFamilyIndices;
		const Mango::LogicalDevice* LogicalDevice;
	};

	class Renderer_ImplVulkan : public Renderer
	{
	public:
		typedef void (*OnResizeCallback)();

		Renderer_ImplVulkan(const Renderer_ImplVulkan_CreateInfo createInfo);
		Renderer_ImplVulkan() = delete;
		Renderer_ImplVulkan(const Renderer_ImplVulkan&) = delete;
		Renderer_ImplVulkan operator=(const Renderer_ImplVulkan&) = delete;

		void BeginFrame(uint32_t currentFrame);
		void EndFrame();
		void HandleResize();

		void DrawRect(glm::mat4 transform, glm::vec4 color) override;
		void DrawTriangle(glm::mat4 transform, glm::vec4 color) override;

		const Mango::SwapChain* GetSwapChain() const { return _swapChain.get(); }
		const Mango::GraphicsPipeline* GetGraphicsPipeline() const { return _graphicsPipeline.get(); }
		void SubmitCommandBuffers(std::vector<Mango::CommandBuffer> commandBuffers);

		void SetViewportInfo(Mango::ViewportInfo info);
		void SetOnResizeCallback(OnResizeCallback callback);

		const Mango::ViewportInfo& GetCurrentViewportInfo() const { return _viewportInfo; }
		const Mango::CommandBuffer& GetCurrentCommandBuffer() const { return _commandBuffers->GetCommandBuffer(_currentFrame); }

	private:
		void UpdateUniformBuffer(glm::mat4 modelMatrix);

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
		std::vector<VkDescriptorSet> _descriptorSets;

		uint32_t _currentFrame = 0;
		Mango::ViewportInfo _viewportInfo;
		OnResizeCallback _onResizeCallback;

		struct RenderData
		{
			std::vector<Mango::Vertex> Vertices;
			std::vector<uint16_t> Indices;

			const std::vector<glm::vec3> TriangleVertices{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f} };
			const std::vector<uint16_t> TriangleIndices{ 0, 1, 2 };

			void Reset()
			{
				Vertices.clear();
				Indices.clear();
			}
		};
		RenderData _renderData{};
		std::unique_ptr<Mango::VertexBuffer> _vertexBuffer;
		std::unique_ptr<Mango::IndexBuffer> _indexBuffer;

		// TODO: Game objects (temporary)
		//const std::vector<Mango::Vertex> _vertices =
		//{
		//	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		//	{{0.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		//	{{-1.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},

		//	//{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		//	//{{1.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		//	//{{1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		//	//{{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}}
		//};
		//const std::vector<uint16_t> _indices =
		//{
		//	0, 1, 2, 3, 4, 5, 5, 6, 3
		//};
		//std::unique_ptr<Mango::VertexBuffer> _vertexBuffer;
		//std::unique_ptr<Mango::IndexBuffer> _indexBuffer;
	};
}
