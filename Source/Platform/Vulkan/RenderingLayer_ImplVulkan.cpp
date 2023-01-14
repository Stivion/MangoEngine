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
    const uint32_t imageCount = _swapChain->GetSwapChainImages().size();
    for (size_t i = 0; i < imageCount; i++)
    {
        Mango::ImageCreateInfo createInfo{};
        createInfo.VulkanContext = _vulkanContext;
        createInfo.ImageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
        createInfo.UsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.Width = _swapChain->GetSwapChainExtent().width;
        createInfo.Height = _swapChain->GetSwapChainExtent().height;
        _images.push_back(std::make_unique<Mango::Image>(createInfo));
    }

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

    _screenRenderArea.X = 0;
    _screenRenderArea.Y = 0;
    _screenRenderArea.Width = _swapChain->GetSwapChainExtent().width;
    _screenRenderArea.Height = _swapChain->GetSwapChainExtent().height;
    _screenRenderAreaInfo.ImageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
    _screenRenderAreaInfo.SurfaceCapabilities = swapChainSupportDetails.SurfaceCapabilities;
    _screenRenderAreaInfo.Images = _swapChain->GetSwapChainImages();
    _screenRenderAreaInfo.ImageViews = _swapChain->GetSwapChainImageViews();
    _viewportRenderArea.X = 0;
    _viewportRenderArea.Y = 0;
    _viewportRenderArea.Width = _swapChain->GetSwapChainExtent().width;
    _viewportRenderArea.Height = _swapChain->GetSwapChainExtent().height;
    _viewportRenderAreaInfo.ImageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
    _viewportRenderAreaInfo.SurfaceCapabilities = swapChainSupportDetails.SurfaceCapabilities;
    _viewportRenderAreaInfo.Images = GetImages(_images);
    _viewportRenderAreaInfo.ImageViews = GetImageViews(_images);

    Mango::Renderer_ImplVulkan_CreateInfo rendererCreateInfo{};
    rendererCreateInfo.MaxFramesInFlight = _maxFramesInFlight;
    rendererCreateInfo.VulkanContext = _vulkanContext;
    rendererCreateInfo.RenderArea = _viewportRenderArea;
    rendererCreateInfo.RenderAreaInfo = _viewportRenderAreaInfo;
    _renderer = std::make_unique<Mango::Renderer_ImplVulkan>(rendererCreateInfo);

    Mango::ImGuiEditor_ImplGLFWVulkan_CreateInfo editorCreateInfo{};
    editorCreateInfo.MaxFramesInFlight = _maxFramesInFlight;
    editorCreateInfo.VulkanContext = _vulkanContext;
    editorCreateInfo.RenderArea = _screenRenderArea;
    editorCreateInfo.RenderAreaInfo = _screenRenderAreaInfo;
    editorCreateInfo.ViewportRenderArea = _viewportRenderArea;
    editorCreateInfo.ViewportAreaInfo = _viewportRenderAreaInfo;
    _editor = std::make_unique<Mango::ImGuiEditor_ImplGLFWVulkan>(editorCreateInfo);
}

void Mango::RenderingLayer_ImplVulkan::BeginFrame()
{
    _editor->NewFrame(_currentFrame);
    _editor->ConstructEditor();
    const auto viewportSize = _editor->GetViewportSize();
    if (viewportSize.x != _viewportRenderArea.Width || viewportSize.y != _viewportRenderArea.Height)
    {
        // Viewport resize will dispose old viewport image, so we must wait for rendering to complete
        WaitRenderingIdle();

        const auto& currentExtent = _swapChain->GetSwapChainExtent();
        _viewportRenderArea.X = 0;
        _viewportRenderArea.Y = 0;
        _viewportRenderArea.Width = static_cast<uint32_t>(std::max(viewportSize.x, 1.0f));
        _viewportRenderArea.Height = static_cast<uint32_t>(std::max(viewportSize.y, 1.0f));

        for (const auto& image : _images)
        {
            const auto imageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
            const auto usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            image->RecreateImage(_viewportRenderArea.Width, _viewportRenderArea.Height, imageFormat, usageFlags);
        }

        _viewportRenderAreaInfo.Images = GetImages(_images);
        _viewportRenderAreaInfo.ImageViews = GetImageViews(_images);

        _renderer->HandleResize(_viewportRenderArea, _viewportRenderAreaInfo);
        _renderer->EndFrame();
        _editor->HandleViewportResize(_viewportRenderArea, _viewportRenderAreaInfo);
        _editor->EndFrame();
        return;
    }
    _editor->EndFrame();

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
    _imageAcquired = true;

    _fences[_currentFrame]->ResetFence();

    _renderer->BeginFrame(_currentFrame);
}

void Mango::RenderingLayer_ImplVulkan::EndFrame()
{
    if (!_imageAcquired)
    {
        return;
    }

    _renderer->EndFrame();

    const uint32_t imageIndex = _swapChain->GetCurrentImageIndex();
    std::vector<VkCommandBuffer> vkCommandBuffers = 
    { 
        _renderer->RecordCommandBuffer(imageIndex).GetVkCommandBuffer(),
        _editor->RecordCommandBuffer(imageIndex).GetVkCommandBuffer()
    };

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
    _imageAcquired = false;
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
    
    const auto& swapChainDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        _vulkanContext->GetPhysicalDevice()->GetDevice(),
        _vulkanContext->GetRenderSurface()->GetRenderSurface()
    );

    const auto& currentExtent = _swapChain->GetSwapChainExtent();
    _screenRenderArea.X = 0;
    _screenRenderArea.Y = 0;
    _screenRenderArea.Width = currentExtent.width;
    _screenRenderArea.Height = currentExtent.height;
    _screenRenderAreaInfo.ImageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
    _screenRenderAreaInfo.Images = _swapChain->GetSwapChainImages();
    _screenRenderAreaInfo.ImageViews = _swapChain->GetSwapChainImageViews();
    _screenRenderAreaInfo.SurfaceCapabilities = swapChainDetails.SurfaceCapabilities;

    const glm::vec2 viewportSize = _editor->GetViewportSize();
    _viewportRenderArea.X = 0;
    _viewportRenderArea.Y = 0;
    _viewportRenderArea.Width = static_cast<uint32_t>(std::max(viewportSize.x, 1.0f));
    _viewportRenderArea.Height = static_cast<uint32_t>(std::max(viewportSize.y, 1.0f));
    
    for (const auto& image : _images)
    {
        const auto imageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
        const auto usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        image->RecreateImage(_viewportRenderArea.Width, _viewportRenderArea.Height, imageFormat, usageFlags);
    }

    _viewportRenderAreaInfo.ImageFormat = _swapChain->GetSwapChainSurfaceFormat().format;
    _viewportRenderAreaInfo.SurfaceCapabilities = swapChainDetails.SurfaceCapabilities;
    _viewportRenderAreaInfo.Images = GetImages(_images);
    _viewportRenderAreaInfo.ImageViews = GetImageViews(_images);

    _renderer->HandleResize(_viewportRenderArea, _viewportRenderAreaInfo);
    _editor->HandleResize(_screenRenderArea, _screenRenderAreaInfo);
    _editor->HandleViewportResize(_viewportRenderArea, _viewportRenderAreaInfo);
}

std::vector<VkImage> Mango::RenderingLayer_ImplVulkan::GetImages(std::vector<std::unique_ptr<Mango::Image>>& images)
{
    std::vector<VkImage> vkImages(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        vkImages[i] = images[i]->Get();
    }
    return vkImages;
}

std::vector<VkImageView> Mango::RenderingLayer_ImplVulkan::GetImageViews(std::vector<std::unique_ptr<Mango::Image>>& images)
{
    std::vector<VkImageView> vkImageViews(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        vkImageViews[i] = images[i]->GetImageView();
    }
    return vkImageViews;
}

void Mango::RenderingLayer_ImplVulkan::WindowResizedCallback(Mango::Window* window, uint32_t width, uint32_t height)
{
    auto* renderingLayer = reinterpret_cast<RenderingLayer_ImplVulkan*>(window->GetWindowUserPointer());
    renderingLayer->FramebufferResized();
}
