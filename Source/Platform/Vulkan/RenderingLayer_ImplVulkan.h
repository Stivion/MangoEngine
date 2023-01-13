#pragma once

#include "../../Render/RenderingLayer.h"

#include "Context.h"
#include "SwapChain.h"
#include "Fence.h"
#include "Semaphore.h"
#include "ICommandBufferRecorder.h"

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

		const uint32_t GetMaxFramesInFlight() const { return _maxFramesInFlight; }
		const uint32_t GetCurrentFrame() const { return _currentFrame; }
		Mango::SwapChain& GetSwapChain() const { return *_swapChain; }

		void SetCommandBufferRecorders(std::vector<Mango::ICommandBufferRecorder*> commandBufferRecorders);

	private:
		const Mango::Context* _vulkanContext;
		std::vector<Mango::ICommandBufferRecorder*> _commandBufferRecorders;

	private:
		std::unique_ptr<Mango::SwapChain> _swapChain;
		std::vector<std::unique_ptr<Mango::Fence>> _fences;
		std::vector<std::unique_ptr<Mango::Semaphore>> _imageAvailableSemaphores;
		std::vector<std::unique_ptr<Mango::Semaphore>> _renderFinishedSemaphores;

	private:
		static void WindowResizedCallback(Mango::Window*, uint32_t width, uint32_t height);

		void FramebufferResized();
	};
}
