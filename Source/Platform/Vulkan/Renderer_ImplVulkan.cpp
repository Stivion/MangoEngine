#include "Renderer_ImplVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Renderer_ImplVulkan::Renderer_ImplVulkan(const Renderer_ImplVulkan_CreateInfo createInfo)
	: Renderer()
{
    _maxFramesInFlight = createInfo.MaxFramesInFlight;
    _instance = createInfo.Instance;
    _physicalDevice = createInfo.PhysicalDevice;
    _queueFamilyIndices = createInfo.QueueFamilyIndices;
    _renderSurface = createInfo.RenderSurface;
    _logicalDevice = createInfo.LogicalDevice;

    Mango::SwapChainSupportDetails swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        _physicalDevice->GetDevice(),
        _renderSurface->GetRenderSurface()
    );
    _swapChain = std::make_unique<Mango::SwapChain>(
        createInfo.WindowFramebufferWidth,
        createInfo.WindowFramebufferHeight,
        *_renderSurface,
        *_logicalDevice,
        swapChainSupportDetails,
        *_queueFamilyIndices
    );

    Mango::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    _renderPass = std::make_unique<Mango::RenderPass>(*_logicalDevice, *_swapChain, renderPassCreateInfo);

    _descriptorPool = std::make_unique<Mango::DescriptorPool>(_poolSizes, *_logicalDevice);

    // All descriptor set layouts is owned by renderer
    {
        Mango::DescriptorSetLayoutBuilder builder(*_logicalDevice);
        _globalDescriptorSetLayout = builder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .Build();
    }
    std::vector<const Mango::DescriptorSetLayout*> layouts{ _globalDescriptorSetLayout.get() };
    _descriptorPool->AllocateDescriptorSets(layouts);
    _graphicsPipeline = std::make_unique<Mango::GraphicsPipeline>(*_logicalDevice, *_renderPass, layouts, "vert.spv", "frag.spv");

    _commandPool = std::make_unique<Mango::CommandPool>(*_logicalDevice, *_queueFamilyIndices);
    _commandBuffers = std::make_unique<Mango::CommandBuffersPool>(
        _maxFramesInFlight,
        *_logicalDevice,
        *_swapChain,
        *_renderPass,
        *_commandPool,
        *_graphicsPipeline
    );
    _framebuffers = std::make_unique<Mango::FramebuffersPool>(*_logicalDevice, *_renderPass, *_swapChain);

    _uniformBuffers = std::make_unique<Mango::UniformBuffersPool>(*_physicalDevice, *_logicalDevice);
    for (uint32_t i = 0; i < _maxFramesInFlight; i++)
    {
        UniformBufferCreateInfo createInfo{};
        createInfo.BufferId = i;
        createInfo.Binding = 0;
        createInfo.BufferSize = sizeof(UniformBufferObject);
        createInfo.DestinationSet = _descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId());
        _uniformBuffers->CreateBuffer(createInfo);
    }

    _fences.resize(_maxFramesInFlight);
    for (size_t i = 0; i < _maxFramesInFlight; i++)
    {
        _fences[i] = std::make_unique<Mango::Fence>(true, *_logicalDevice);
    }

    _imageAvailableSemaphores.resize(_maxFramesInFlight);
    _renderFinishedSemaphores.resize(_maxFramesInFlight);
    for (size_t i = 0; i < _maxFramesInFlight; i++)
    {
        _imageAvailableSemaphores[i] = std::make_unique<Mango::Semaphore>(*_logicalDevice);
        _renderFinishedSemaphores[i] = std::make_unique<Mango::Semaphore>(*_logicalDevice);
    }

    _descriptorSets.clear();
    _descriptorSets.push_back(_descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId()));

    // Set default viewport info
    const auto& currentExtent = _swapChain->GetSwapChainExtent();
    _viewportInfo.X = 0;
    _viewportInfo.Y = 0;
    _viewportInfo.Width = currentExtent.width;
    _viewportInfo.Height = currentExtent.height;
    _viewportInfo.MinDepth = 0;
    _viewportInfo.MaxDepth = 1;
}

void Mango::Renderer_ImplVulkan::BeginFrame(uint32_t currentFrame)
{
    _currentFrame = currentFrame % _maxFramesInFlight;
    _fences[_currentFrame]->WaitForFence();

    // Acquire next image to render to
    const auto nextImageResult = _swapChain->AcquireNextImage(*_imageAvailableSemaphores[_currentFrame]);
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        HandleResize();
        return;
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    _fences[_currentFrame]->ResetFence();

    const auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);
    const auto& currentFramebuffer = _framebuffers->GetFramebuffer(_swapChain->GetCurrentImageIndex());

    currentCommandBuffer.Reset();
    currentCommandBuffer.BeginCommandBuffer();
    currentCommandBuffer.BeginRenderPass(
        currentFramebuffer.GetSwapChainFramebuffer(),
        _graphicsPipeline->GetGraphicsPipeline(),
        _viewportInfo
    );
}

void Mango::Renderer_ImplVulkan::EndFrame()
{
    const auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);

    // Group all verices into buffers
    const uint32_t vertexCount = static_cast<uint32_t>(_renderData.Vertices.size());
    const uint32_t indicesCount = static_cast<uint32_t>(_renderData.Indices.size());
    _vertexBuffer = std::make_unique<Mango::VertexBuffer>(vertexCount, sizeof(Vertex) * vertexCount, _renderData.Vertices.data(), *_physicalDevice, *_logicalDevice, *_commandPool);
    _indexBuffer = std::make_unique<Mango::IndexBuffer>(indicesCount, sizeof(uint16_t) * indicesCount, _renderData.Indices.data(), *_physicalDevice, *_logicalDevice, *_commandPool);
    _renderData.Reset();

    // Draw call
    currentCommandBuffer.DrawIndexed(*_vertexBuffer, *_indexBuffer, _descriptorSets);
    
    // End frame
    currentCommandBuffer.EndRenderPass();
    currentCommandBuffer.EndCommandBuffer();
}

void Mango::Renderer_ImplVulkan::HandleResize()
{
    M_TRACE("Swap chain is out of date. Recreating.");
    // Finish all rendering before recrating swap chain
    for (const auto& fence : _fences)
    {
        fence->WaitForFence();
    }

    auto vkPhysicalDevice = _physicalDevice->GetDevice();
    auto vkRenderSurface = _renderSurface->GetRenderSurface();
    auto swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(vkPhysicalDevice, vkRenderSurface);
    auto width = std::numeric_limits<uint32_t>::max();
    auto height = std::numeric_limits<uint32_t>::max();

    _swapChain->RecreateSwapChain(width, height, swapChainSupportDetails, *_queueFamilyIndices);
    _renderPass->RecreateRenderPass(*_logicalDevice, *_swapChain);

    const auto& imageViews = _swapChain->GetSwapChainImageViews();
    M_ASSERT(imageViews.size() == _framebuffers->GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        _framebuffers->GetFramebuffer(i).RecreateFramebuffer(*_renderPass, _swapChain->GetSwapChainExtent(), imageViews[i]);
    }

    const auto& currentExtent = _swapChain->GetSwapChainExtent();
    _viewportInfo.X = 0;
    _viewportInfo.Y = 0;
    _viewportInfo.Width = currentExtent.width;
    _viewportInfo.Height = currentExtent.height;
    _viewportInfo.MinDepth = 0;
    _viewportInfo.MaxDepth = 1;
    _onResizeCallback();
}

void Mango::Renderer_ImplVulkan::DrawRect(glm::mat4 transform, glm::vec4 color)
{
    // TODO: Write implmentation
}

void Mango::Renderer_ImplVulkan::DrawTriangle(glm::mat4 transform, glm::vec4 color)
{
    const uint32_t vertexCount = static_cast<uint32_t>(_renderData.TriangleVertices.size());

    const uint32_t lastVertexCount = _renderData.Vertices.size();
    for (size_t i = 0; i < vertexCount; i++)
    {
        Mango::Vertex vertex{ _renderData.TriangleVertices[i], color };
        _renderData.Vertices.push_back(vertex);
        _renderData.Indices.push_back(lastVertexCount + i); // 0, 1, 2 ... 3, 4, 5 ... 6, 7, 8 ...
    }
    UpdateUniformBuffer(transform);
}

void Mango::Renderer_ImplVulkan::SubmitCommandBuffers(std::vector<Mango::CommandBuffer> commandBuffers)
{
    // Submit command buffer to graphics queue
    std::vector<VkCommandBuffer> vkCommandBuffers;
    for (const auto& buffer : commandBuffers)
    {
        vkCommandBuffers.push_back(buffer.GetVkCommandBuffer());
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

    auto queueSubmitResult = vkQueueSubmit(_logicalDevice->GetGraphicsQueue(), 1, &submitInfo, _fences[_currentFrame]->GetFence());
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

    auto presentImageResult = vkQueuePresentKHR(_logicalDevice->GetPresentationQueue(), &presentInfo);
    if (presentImageResult == VK_ERROR_OUT_OF_DATE_KHR || presentImageResult == VK_SUBOPTIMAL_KHR)
    {
        HandleResize();
    }
    else
    {
        M_ASSERT(presentImageResult == VK_SUCCESS && "Failed to acquire present swap chain image");
    }
}

void Mango::Renderer_ImplVulkan::SetViewportInfo(Mango::ViewportInfo info)
{
    _viewportInfo = info;
}

void Mango::Renderer_ImplVulkan::SetOnResizeCallback(OnResizeCallback callback)
{
    _onResizeCallback = callback;
}

void Mango::Renderer_ImplVulkan::UpdateUniformBuffer(glm::mat4 modelMatrix)
{
    const auto& currentExtent = _swapChain->GetSwapChainExtent();

    Mango::UniformBufferObject ubo{};
    ubo.Model = modelMatrix; // TODO: We need separate buffer for Model matrix, because it will be updated every frame
    // And this matrices may not be updated every frame
    ubo.View = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.Projection = glm::perspective(glm::radians(45.0f), _viewportInfo.Width / static_cast<float>(_viewportInfo.Height), 0.1f, 10.0f);
    ubo.Projection[1][1] *= -1;

    const auto& uniformBuffer = _uniformBuffers->GetUniformBuffer(_currentFrame);
    void* gpuMemory = uniformBuffer.MapMemory();
    memcpy(gpuMemory, &ubo, sizeof(ubo));
    uniformBuffer.UnmapMemory();
}
