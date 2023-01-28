#pragma once

#include "../../Render/Renderer.h"

#include "Context.h"
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
#include "RenderArea.h"

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
		Mango::RenderArea RenderArea;
		Mango::RenderAreaInfo RenderAreaInfo;
		const Mango::Context* VulkanContext;
	};

	class Renderer_ImplVulkan : public Renderer
	{
	public:
		Renderer_ImplVulkan(const Renderer_ImplVulkan_CreateInfo createInfo);
		Renderer_ImplVulkan() = delete;
		Renderer_ImplVulkan(const Renderer_ImplVulkan&) = delete;
		Renderer_ImplVulkan operator=(const Renderer_ImplVulkan&) = delete;

		void BeginFrame(uint32_t currentFrame);
		void EndFrame();
		void HandleResize(Mango::RenderArea renderArea, Mango::RenderAreaInfo renderAreaInfo);

		const Mango::CommandBuffer& RecordCommandBuffer(uint32_t imageIndex);

		void DrawRect(glm::mat4 transform, glm::vec4 color) override;
		void DrawTriangle(glm::mat4 transform, glm::vec4 color) override;

		void SetCamera(RendererCameraInfo cameraInfo) override;

	private:
		void UpdateGlobalDescriptorSets();
		void UpdatePerModelDescriptorSets();

	private:
		uint32_t _maxFramesInFlight;
		const Mango::Context* _vulkanContext;

	private:
		std::unique_ptr<Mango::RenderPass> _renderPass;
		std::unique_ptr<Mango::DescriptorSetLayout> _globalDescriptorSetLayout;
		std::unique_ptr<Mango::DescriptorSetLayout> _perModelDescriptorSetLayout;
		const std::vector<VkDescriptorPoolSize> _poolSizes = // TODO: Figure out how to allocate correct pool sizes
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 2 }
		};
		std::unique_ptr<Mango::DescriptorPool> _descriptorPool;
		std::unique_ptr<Mango::GraphicsPipeline> _graphicsPipeline;
		std::unique_ptr<Mango::UniformBuffersPool> _uniformBuffers;
		std::unique_ptr<Mango::FramebuffersPool> _framebuffers;
		std::unique_ptr<Mango::CommandPool> _commandPool;
		std::unique_ptr<Mango::CommandBuffersPool> _commandBuffers;
		std::vector<VkDescriptorSet> _descriptorSets;

		uint32_t _currentFrame = 0;
		Mango::RenderArea _renderArea;
		Mango::RenderAreaInfo _renderAreaInfo;

		struct RenderData
		{
			std::vector<glm::mat4> Transforms;
			std::vector<Mango::Vertex> Vertices;
			std::vector<uint16_t> Indices;
			std::vector<uint32_t> DynamicOffsets;
			std::vector<uint32_t> IndicesPerDraw;

			const std::vector<glm::vec3> RectangleVertices{ {-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f } };
			const std::vector<uint16_t> RectangleIndices{ 0, 1, 2, 0, 2, 3 };
			const std::vector<glm::vec3> TriangleVertices{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f} };
			const std::vector<uint16_t> TriangleIndices{ 0, 1, 2 };

			void Reset()
			{
				Vertices.clear();
				Indices.clear();
				Transforms.clear();
				DynamicOffsets.clear();
				IndicesPerDraw.clear();
			}
		};
		RenderData _renderData{};
		std::unique_ptr<Mango::VertexBuffer> _vertexBuffer;
		std::unique_ptr<Mango::IndexBuffer> _indexBuffer;

		RendererCameraInfo _cameraInfo;
	};
}
