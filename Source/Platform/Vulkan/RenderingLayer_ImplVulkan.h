#pragma once

#include "../../Render/RenderingLayer.h"

#include "../../Render/Renderer.h"
#include "../ImGui/ImGuiEditor_ImplGLFWVulkan.h"

#include "Context.h"
#include "SwapChain.h"
#include "Fence.h"
#include "Semaphore.h"
#include "Renderer_ImplVulkan.h"
#include "Image.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Mango
{
	class RenderingLayer_ImplVulkan : public RenderingLayer
	{
	public:
		RenderingLayer_ImplVulkan(const Mango::Context* vulkanContext);
		RenderingLayer_ImplVulkan() = delete;
		RenderingLayer_ImplVulkan(const RenderingLayer_ImplVulkan&) = delete;
		RenderingLayer_ImplVulkan operator=(const RenderingLayer_ImplVulkan&) = delete;

		void BeginFrame() override;
		void EndFrame() override;
		void WaitRenderingIdle();

		Mango::Renderer& GetRenderer() const { return *_renderer; }
		const uint32_t GetMaxFramesInFlight() const { return _maxFramesInFlight; }
		const uint32_t GetCurrentFrame() const { return _currentFrame; }
		Mango::SwapChain& GetSwapChain() const { return *_swapChain; }

	private:
		const Mango::Context* _vulkanContext;

	private:
		std::unique_ptr<Mango::Renderer_ImplVulkan> _renderer;
		std::unique_ptr<Mango::ImGuiEditor_ImplGLFWVulkan> _editor;
		std::unique_ptr<Mango::SwapChain> _swapChain;
		std::vector<std::unique_ptr<Mango::Image>> _images;
		std::vector<std::unique_ptr<Mango::Fence>> _fences;
		std::vector<std::unique_ptr<Mango::Semaphore>> _imageAvailableSemaphores;
		std::vector<std::unique_ptr<Mango::Semaphore>> _renderFinishedSemaphores;

		Mango::RenderArea _screenRenderArea{};
		Mango::RenderAreaInfo _screenRenderAreaInfo{};
		Mango::RenderArea _viewportRenderArea{};
		Mango::RenderAreaInfo _viewportRenderAreaInfo{};

		bool _imageAcquired = false;

	private:
		static void WindowResizedCallback(Mango::Window*, uint32_t width, uint32_t height);

		void FramebufferResized();
		std::vector<VkImage> GetImages(std::vector<std::unique_ptr<Mango::Image>>& images);
		std::vector<VkImageView> GetImageViews(std::vector<std::unique_ptr<Mango::Image>>& images);
	};
}
