#include "Application.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();
}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::InitializeWindow()
{
    _window = std::make_unique<Mango::GLFWWindow>(1200, 800);
}

void Mango::Application::InitializeVulkan()
{
    _vulkanContext = std::make_unique<const Mango::Context>(_window.get());
    _renderingLayer = std::make_unique<Mango::RenderingLayer_ImplVulkan>(_vulkanContext.get());

    // Initialize with default values. It will change after rendering started
    const auto& swapChain = _renderingLayer->GetSwapChain();
    const auto& currentExtent = swapChain.GetSwapChainExtent();
    _windowRenderArea.X = 0;
    _windowRenderArea.Y = 0;
    _windowRenderArea.Width = currentExtent.width;
    _windowRenderArea.Height = currentExtent.height;
    _viewportRenderArea.X = 0;
    _viewportRenderArea.Y = 0;
    _viewportRenderArea.Width = currentExtent.width;
    _viewportRenderArea.Height = currentExtent.height;

    const auto& swapChainDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(_vulkanContext->GetPhysicalDevice()->GetDevice(), _vulkanContext->GetRenderSurface()->GetRenderSurface());
    _renderAreaInfo.ImageFormat = swapChain.GetSwapChainSurfaceFormat().format;
    _renderAreaInfo.Images = swapChain.GetSwapChainImages();
    _renderAreaInfo.ImageViews = swapChain.GetSwapChainImageViews();
    _renderAreaInfo.SurfaceCapabilities = swapChainDetails.SurfaceCapabilities;

    InitializeViewportImage(_viewportRenderArea.Width, _viewportRenderArea.Height);
    InitializeViewportImageView();
    _viewportRenderAreaInfo.ImageFormat = swapChain.GetSwapChainSurfaceFormat().format;
    _viewportRenderAreaInfo.SurfaceCapabilities = swapChainDetails.SurfaceCapabilities;

    Mango::Renderer_ImplVulkan_CreateInfo rendererCreateInfo{};
    rendererCreateInfo.MaxFramesInFlight = _renderingLayer->GetMaxFramesInFlight();
    rendererCreateInfo.VulkanContext = _vulkanContext.get();
    rendererCreateInfo.RenderArea = &_windowRenderArea;
    rendererCreateInfo.RenderAreaInfo = &_viewportRenderAreaInfo;
    _renderer = std::make_unique<Mango::Renderer_ImplVulkan>(rendererCreateInfo);

    Mango::ImGuiEditor_ImplGLFWVulkan_CreateInfo editorCreateInfo{};
    editorCreateInfo.MaxFramesInFlight = _renderingLayer->GetMaxFramesInFlight();
    editorCreateInfo.VulkanContext = _vulkanContext.get();
    editorCreateInfo.RenderArea = &_windowRenderArea;
    editorCreateInfo.RenderAreaInfo = &_renderAreaInfo;
    editorCreateInfo.ViewportRenderArea = &_viewportRenderArea;
    editorCreateInfo.ViewportAreaInfo = &_viewportRenderAreaInfo;
    _editor = std::make_unique<Mango::ImGuiEditor_ImplGLFWVulkan>(editorCreateInfo);
    
    std::vector<Mango::ICommandBufferRecorder*> commandBufferRecorders = { _renderer.get(), _editor.get() };
    _renderingLayer->SetCommandBufferRecorders(commandBufferRecorders);
    _renderingLayer->SetUserDefinedCallbackPointer(this);
    _renderingLayer->SetFramebufferResizedCallback(FramebufferResizedCallback);
}

void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!_window->ShouldClose())
    {
        glfwPollEvents(); // TODO: WAT
        DrawFrame();
    }

    vkDeviceWaitIdle(_vulkanContext->GetLogicalDevice()->GetDevice()); // TODO: Remove
}

void Mango::Application::DrawFrame()
{
    _editor->NewFrame(_renderingLayer->GetCurrentFrame());
    _editor->ConstructEditor();
    const auto viewportSize = _editor->GetViewportSize();
    if (viewportSize.x != _viewportRenderArea.Width || viewportSize.y != _viewportRenderArea.Height)
    {
        const auto& currentExtent = _renderingLayer->GetSwapChain().GetSwapChainExtent();
        _viewportRenderArea.Width = static_cast<uint32_t>(viewportSize.x);
        _viewportRenderArea.Height = static_cast<uint32_t>(viewportSize.y);
        InitializeViewportImage(_viewportRenderArea.Width, _viewportRenderArea.Height);
        InitializeViewportImageView();

        // Viewport resize will dispose old viewport image, so we must wait for rendering to complete
        _renderingLayer->WaitRenderingIdle();
        _renderer->HandleResize(_viewportRenderArea, _viewportRenderAreaInfo);
        _editor->HandleViewportResize(&_viewportRenderArea);
        _editor->EndFrame();
        return;
    }
    _editor->EndFrame();

    // ImGuiEditor expects that Renderer image is avalilable
    // So we must BeginFrame before recording command buffers
    _renderingLayer->BeginFrame();
    _renderer->BeginFrame(_renderingLayer->GetCurrentFrame());

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    _renderer->DrawTriangle(transform, { 1.0f, 1.0f, 0.0f, 1.0f });
    //transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
    //    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    //    * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    //_renderer->DrawTriangle(transform, { 0.0f, 1.0f, 1.0f, 1.0f });

    _renderer->EndFrame();

    _renderingLayer->EndFrame();
}

void Mango::Application::FramebufferResizedCallback(void* applicationPtr)
{
    const auto& applicationPointer = reinterpret_cast<Application*>(applicationPtr);

    const auto& swapChainDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        applicationPointer->_vulkanContext->GetPhysicalDevice()->GetDevice(),
        applicationPointer->_vulkanContext->GetRenderSurface()->GetRenderSurface()
    );

    const auto& swapChain = applicationPointer->_renderingLayer->GetSwapChain();
    const auto& currentExtent = swapChain.GetSwapChainExtent();

    applicationPointer->_windowRenderArea.X = 0;
    applicationPointer->_windowRenderArea.Y = 0;
    applicationPointer->_windowRenderArea.Width = currentExtent.width;
    applicationPointer->_windowRenderArea.Height = currentExtent.height;

    const glm::vec2 viewportSize = applicationPointer->_editor->GetViewportSize();
    applicationPointer->_viewportRenderArea.X = 0;
    applicationPointer->_viewportRenderArea.Y = 0;
    applicationPointer->_viewportRenderArea.Width = std::min(static_cast<uint32_t>(viewportSize.x), currentExtent.width);
    applicationPointer->_viewportRenderArea.Height = std::min(static_cast<uint32_t>(viewportSize.y), currentExtent.height);

    applicationPointer->_renderAreaInfo.ImageFormat = swapChain.GetSwapChainSurfaceFormat().format;
    applicationPointer->_renderAreaInfo.Images = swapChain.GetSwapChainImages();
    applicationPointer->_renderAreaInfo.ImageViews = swapChain.GetSwapChainImageViews();
    applicationPointer->_renderAreaInfo.SurfaceCapabilities = swapChainDetails.SurfaceCapabilities;

    // Same _renderAreaInfo for both _renderer and _editor
    // It is incorrect because they should have different images
    applicationPointer->_renderer->HandleResize(applicationPointer->_viewportRenderArea, applicationPointer->_viewportRenderAreaInfo);
    applicationPointer->_editor->HandleResize(applicationPointer->_windowRenderArea, applicationPointer->_renderAreaInfo);
    applicationPointer->_editor->HandleViewportResize(&applicationPointer->_viewportRenderArea);
}

void Mango::Application::InitializeViewportImage(uint32_t width, uint32_t height)
{
    std::vector<VkImage> images(3);
    {
        VkImage image;
        const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
        VkExtent2D currentExtent{};
        currentExtent.width = width;
        currentExtent.height = height;

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        info.extent.width = currentExtent.width;
        info.extent.height = currentExtent.height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        auto createImageResult = vkCreateImage(vkLogicalDevice, &info, nullptr, &image);
        M_TRACE("Create viewport image result is: " + std::to_string(createImageResult));
        M_ASSERT(createImageResult == VK_SUCCESS && "Failed to create viewport image");

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(vkLogicalDevice, image, &memoryRequirements);

        VkDeviceMemory imageMemory;
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = _vulkanContext->GetPhysicalDevice()->FindSuitableMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto allocateImageMemoryResult = vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &imageMemory);
        M_TRACE("Allocate viewport image memory result is: " + std::to_string(allocateImageMemoryResult));
        M_ASSERT(allocateImageMemoryResult == VK_SUCCESS && "Failed to allocate viewport image memory");

        auto bindImageMemoryResult = vkBindImageMemory(vkLogicalDevice, image, imageMemory, 0);
        M_TRACE("Bind viewport image memory result is: " + std::to_string(bindImageMemoryResult));
        M_ASSERT(bindImageMemoryResult == VK_SUCCESS && "Failed to bind viewport image memory");
        images[0] = image;
    }

    {
        VkImage image;
        const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
        VkExtent2D currentExtent{};
        currentExtent.width = width;
        currentExtent.height = height;

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        info.extent.width = currentExtent.width;
        info.extent.height = currentExtent.height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        auto createImageResult = vkCreateImage(vkLogicalDevice, &info, nullptr, &image);
        M_TRACE("Create viewport image result is: " + std::to_string(createImageResult));
        M_ASSERT(createImageResult == VK_SUCCESS && "Failed to create viewport image");

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(vkLogicalDevice, image, &memoryRequirements);

        VkDeviceMemory imageMemory;
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = _vulkanContext->GetPhysicalDevice()->FindSuitableMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto allocateImageMemoryResult = vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &imageMemory);
        M_TRACE("Allocate viewport image memory result is: " + std::to_string(allocateImageMemoryResult));
        M_ASSERT(allocateImageMemoryResult == VK_SUCCESS && "Failed to allocate viewport image memory");

        auto bindImageMemoryResult = vkBindImageMemory(vkLogicalDevice, image, imageMemory, 0);
        M_TRACE("Bind viewport image memory result is: " + std::to_string(bindImageMemoryResult));
        M_ASSERT(bindImageMemoryResult == VK_SUCCESS && "Failed to bind viewport image memory");
        images[1] = image;
    }

    {
        VkImage image;
        const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
        VkExtent2D currentExtent{};
        currentExtent.width = width;
        currentExtent.height = height;

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        info.extent.width = currentExtent.width;
        info.extent.height = currentExtent.height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        auto createImageResult = vkCreateImage(vkLogicalDevice, &info, nullptr, &image);
        M_TRACE("Create viewport image result is: " + std::to_string(createImageResult));
        M_ASSERT(createImageResult == VK_SUCCESS && "Failed to create viewport image");

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(vkLogicalDevice, image, &memoryRequirements);

        VkDeviceMemory imageMemory;
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = _vulkanContext->GetPhysicalDevice()->FindSuitableMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto allocateImageMemoryResult = vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &imageMemory);
        M_TRACE("Allocate viewport image memory result is: " + std::to_string(allocateImageMemoryResult));
        M_ASSERT(allocateImageMemoryResult == VK_SUCCESS && "Failed to allocate viewport image memory");

        auto bindImageMemoryResult = vkBindImageMemory(vkLogicalDevice, image, imageMemory, 0);
        M_TRACE("Bind viewport image memory result is: " + std::to_string(bindImageMemoryResult));
        M_ASSERT(bindImageMemoryResult == VK_SUCCESS && "Failed to bind viewport image memory");
        images[2] = image;
    }

    _viewportRenderAreaInfo.Images = images;
}

void Mango::Application::InitializeViewportImageView()
{
    std::vector<VkImageView> imageViews(3);

    {
        VkImageView imageView;
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _viewportRenderAreaInfo.Images[0];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        auto createImageViewResult = vkCreateImageView(_vulkanContext->GetLogicalDevice()->GetDevice(), &imageViewCreateInfo, nullptr, &imageView);
        M_TRACE("Create viewport image view result is: " + std::to_string(createImageViewResult));
        M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create viewport image view result");
        imageViews[0] = imageView;
    }

    {
        VkImageView imageView;
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _viewportRenderAreaInfo.Images[1];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        auto createImageViewResult = vkCreateImageView(_vulkanContext->GetLogicalDevice()->GetDevice(), &imageViewCreateInfo, nullptr, &imageView);
        M_TRACE("Create viewport image view result is: " + std::to_string(createImageViewResult));
        M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create viewport image view result");
        imageViews[1] = imageView;
    }

    {
        VkImageView imageView;
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _viewportRenderAreaInfo.Images[2];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: This is format from swap chain
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        auto createImageViewResult = vkCreateImageView(_vulkanContext->GetLogicalDevice()->GetDevice(), &imageViewCreateInfo, nullptr, &imageView);
        M_TRACE("Create viewport image view result is: " + std::to_string(createImageViewResult));
        M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create viewport image view result");
        imageViews[2] = imageView;
    }

    _viewportRenderAreaInfo.ImageViews = imageViews;
}
