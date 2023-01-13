#include "RenderingLayer_ImplVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::RenderingLayer_ImplVulkan::RenderingLayer_ImplVulkan(const Mango::Context* vulkanContext)
    : _vulkanContext(vulkanContext)
{
    _currentFrame = 0;
    _maxFramesInFlight = 2;
    _vulkanContext = vulkanContext;

    _vulkanContext->GetWindow()->SetWindowUserPointer(this);
    _vulkanContext->GetWindow()->SetFramebufferResizedCallback(WindowResizedCallback);

    Mango::SwapChainSupportDetails swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        _vulkanContext->GetPhysicalDevice()->GetDevice(),
        _vulkanContext->GetRenderSurface()->GetRenderSurface()
    );

    // Swap chain must always match window framebuffer size
    _swapChain = std::make_unique<Mango::SwapChain>(
        swapChainSupportDetails.SurfaceCapabilities.currentExtent.width,
        swapChainSupportDetails.SurfaceCapabilities.currentExtent.height,
        *_vulkanContext->GetRenderSurface(),
        *_vulkanContext->GetLogicalDevice(),
        swapChainSupportDetails,
        *_vulkanContext->GetQueueFamilyIndices()
    );

    _fences.resize(_maxFramesInFlight);
    for (size_t i = 0; i < _maxFramesInFlight; i++)
    {
        _fences[i] = std::make_unique<Mango::Fence>(true, *_vulkanContext->GetLogicalDevice());
    }

    _imageAvailableSemaphores.resize(_maxFramesInFlight);
    _renderFinishedSemaphores.resize(_maxFramesInFlight);
    for (size_t i = 0; i < _maxFramesInFlight; i++)
    {
        _imageAvailableSemaphores[i] = std::make_unique<Mango::Semaphore>(*_vulkanContext->GetLogicalDevice());
        _renderFinishedSemaphores[i] = std::make_unique<Mango::Semaphore>(*_vulkanContext->GetLogicalDevice());
    }
}

void Mango::RenderingLayer_ImplVulkan::BeginFrame()
{
    _fences[_currentFrame]->WaitForFence();

    const auto nextImageResult = _swapChain->AcquireNextImage(*_imageAvailableSemaphores[_currentFrame]);
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        FramebufferResized();
        return;
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    _fences[_currentFrame]->ResetFence();
}

void Mango::RenderingLayer_ImplVulkan::EndFrame()
{
    // Is is a callers responsibility to set command buffers before calling EndFrame
    M_ASSERT(_commandBufferRecorders.size() > 0 && "Command buffer recorders is empty");

    std::vector<VkCommandBuffer> vkCommandBuffers;
    for (ICommandBufferRecorder* recorder : _commandBufferRecorders)
    {
        vkCommandBuffers.push_back(recorder->RecordCommandBuffer(_swapChain->GetCurrentImageIndex()).GetVkCommandBuffer());
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = vkCommandBuffers.data();

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]->GetSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]->GetSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto queueSubmitResult = vkQueueSubmit(_vulkanContext->GetLogicalDevice()->GetGraphicsQueue(), 1, &submitInfo, _fences[_currentFrame]->GetFence());
    M_ASSERT(queueSubmitResult == VK_SUCCESS && "Failed to submit command buffers to queue");

    // Submit rendered image to presentation queue
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const uint32_t imageIndex = _swapChain->GetCurrentImageIndex();
    VkSwapchainKHR swapChains[] = { _swapChain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    auto presentImageResult = vkQueuePresentKHR(_vulkanContext->GetLogicalDevice()->GetPresentationQueue(), &presentInfo);
    if (presentImageResult == VK_ERROR_OUT_OF_DATE_KHR || presentImageResult == VK_SUBOPTIMAL_KHR)
    {
        FramebufferResized();
    }
    else
    {
        M_ASSERT(presentImageResult == VK_SUCCESS && "Failed to acquire present swap chain image");
    }
    _currentFrame = (_currentFrame + 1) % _maxFramesInFlight;
}

void Mango::RenderingLayer_ImplVulkan::WaitRenderingIdle()
{
    //vkDeviceWaitIdle(_logicalDevice->GetDevice());
    for (const auto& fence : _fences)
    {
        if (fence->IsFenceSignaled())
        {
            fence->WaitForFence();
        }
    }
}

void Mango::RenderingLayer_ImplVulkan::SetCommandBufferRecorders(std::vector<Mango::ICommandBufferRecorder*> commandBufferRecorders)
{
    _commandBufferRecorders = commandBufferRecorders;
}

void Mango::RenderingLayer_ImplVulkan::FramebufferResized()
{
    // Handles resize of the window framebuffer that was detected by Vulkan pipeline
    M_TRACE("Swap chain is out of date. Recreating.");
    
    // Finish all rendering before recrating swap chain
    for (const auto& fence : _fences)
    {
        fence->WaitForFence();
    }

    auto vkPhysicalDevice = _vulkanContext->GetPhysicalDevice()->GetDevice();
    auto vkRenderSurface = _vulkanContext->GetRenderSurface()->GetRenderSurface();
    auto swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(vkPhysicalDevice, vkRenderSurface);
    
    // SwapChain must always match window framebuffer size
    auto width = swapChainSupportDetails.SurfaceCapabilities.currentExtent.width;
    auto height = swapChainSupportDetails.SurfaceCapabilities.currentExtent.height;

    _swapChain->RecreateSwapChain(width, height, swapChainSupportDetails, *_vulkanContext->GetQueueFamilyIndices());
    
    // After swap chain recreation we want to call all registered callback to handle consumer code for resizing
    if (_framebufferResizedCallback != nullptr)
    {
        _framebufferResizedCallback(_userPointer);
    }
}

void Mango::RenderingLayer_ImplVulkan::WindowResizedCallback(Mango::Window* window, uint32_t width, uint32_t height)
{
    auto* renderingLayer = reinterpret_cast<RenderingLayer_ImplVulkan*>(window->GetWindowUserPointer());
    renderingLayer->FramebufferResized();
}
