#include "VulkanRenderer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

Mango::VulkanRenderer::VulkanRenderer(bool isOffscreen, const VulkanRendererCreateInfo createInfo)
	: Renderer(), _hardwareInfo(createInfo.HardwareInfo), _renderSurface(createInfo.RenderSurface), _logicalDevice(createInfo.LogicalDevice), _swapChain(createInfo.SwapChain), _maxFramesInFlight(createInfo.MaxFramesInFlight)
{
    _isOffscreen = isOffscreen;
	// Create render pass
	if (isOffscreen)
	{
        _renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        _renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	}
	else
	{
        _renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        _renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	_renderPass = std::make_unique<Mango::RenderPass>(createInfo.LogicalDevice, createInfo.SwapChain, _renderPassCreateInfo);

    // Create descriptor pool
    _descriptorPool = std::make_unique<Mango::DescriptorPool>(_poolSizes, _logicalDevice);

    // Create descriptor set layouts and allocate them from descriptor set pool
    // All descriptor set layouts is owned by renderer
    {
        Mango::DescriptorSetLayoutBuilder builder(_logicalDevice);
        _globalDescriptorSetLayout = builder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .Build();
    }

    std::vector<const Mango::DescriptorSetLayout*> layouts{ _globalDescriptorSetLayout.get() };
    _descriptorPool->AllocateDescriptorSets(layouts);

    // Create graphics pipeline
    _graphicsPipeline = std::make_unique<Mango::GraphicsPipeline>(_logicalDevice, *_renderPass, layouts, "vert.spv", "frag.spv");

    // Create uniform buffers pool and bind buffers
    _uniformBuffers = std::make_unique<Mango::UniformBuffersPool>(_hardwareInfo.PhysicalDevice, _logicalDevice);
    for (uint32_t i = 0; i < _maxFramesInFlight; i++)
    {
        UniformBufferCreateInfo createInfo{};
        createInfo.BufferId = i;
        createInfo.Binding = 0;
        createInfo.BufferSize = sizeof(UniformBufferObject);
        createInfo.DestinationSet = _descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId());
        uint64_t bufferId = _uniformBuffers->CreateBuffer(createInfo);
    }

    // Create framebuffers
    _framebuffers = std::make_unique<Mango::FramebuffersPool>(_logicalDevice, *_renderPass, _swapChain);

    // Create command buffers
    _commandPool = std::make_unique<Mango::CommandPool>(_logicalDevice, _hardwareInfo.QueueFamilyIndices);
    _commandBuffers = std::make_unique<Mango::CommandBuffersPool>(_maxFramesInFlight, _logicalDevice, _swapChain, *_renderPass, *_commandPool, *_graphicsPipeline);

	// Create fences
	_fences.resize(_maxFramesInFlight);
	for (size_t i = 0; i < _maxFramesInFlight; i++)
	{
		_fences[i] = std::make_unique<Mango::Fence>(true, _logicalDevice);
	}

	// Create semaphores
	_imageAvailableSemaphores.resize(_maxFramesInFlight);
	_renderFinishedSemaphores.resize(_maxFramesInFlight);
	for (size_t i = 0; i < _maxFramesInFlight; i++)
	{
		_imageAvailableSemaphores[i] = std::make_unique<Mango::Semaphore>(_logicalDevice);
		_renderFinishedSemaphores[i] = std::make_unique<Mango::Semaphore>(_logicalDevice);
	}

    // Prepare vertex and index buffers (temporary)
    const uint32_t vertexCount = static_cast<uint32_t>(_vertices.size());
    const uint32_t indicesCount = static_cast<uint32_t>(_indices.size());
    _vertexBuffer = std::make_unique<Mango::VertexBuffer>(vertexCount, sizeof(Vertex) * vertexCount, _vertices.data(), _hardwareInfo.PhysicalDevice, _logicalDevice, *_commandPool);
    _indexBuffer = std::make_unique<Mango::IndexBuffer>(indicesCount, sizeof(uint16_t) * indicesCount, _indices.data(), _hardwareInfo.PhysicalDevice, _logicalDevice, *_commandPool);
}

Mango::VulkanRenderer::~VulkanRenderer()
{
    _renderPass = nullptr;

    for (size_t i = 0; i < _maxFramesInFlight; i++)
    {
        _fences[i] = nullptr;
        _imageAvailableSemaphores[i] = nullptr;
        _renderFinishedSemaphores[i] = nullptr;
    }
}

void Mango::VulkanRenderer::Draw()
{
    this->Draw(*_vertexBuffer, *_indexBuffer);
}

void Mango::VulkanRenderer::Draw(Mango::VertexBuffer& vertexBuffer, Mango::IndexBuffer& indexBuffer)
{
	const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    const auto& vkSwapChain = _swapChain.GetSwapChain();

    _fences[_currentFrame]->WaitForFence();
    
    // Acquire next image to render to
    uint32_t imageIndex;
    auto nextImageResult = vkAcquireNextImageKHR(
        vkLogicalDevice,
        vkSwapChain,
        UINT64_MAX,
        _imageAvailableSemaphores[_currentFrame]->GetSemaphore(),
        VK_NULL_HANDLE,
        &imageIndex
    );
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        // TODO: Handle minimized window
        HandleFramebuffersResized();
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    _fences[_currentFrame]->ResetFence();

    auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);
    currentCommandBuffer.Reset();

    const auto& currentExtent = _swapChain.GetSwapChainExtent();
    UpdateUniformBuffer(_currentFrame);

    Mango::ViewportInfo viewportInfo{};
    viewportInfo.X = 0;
    viewportInfo.Y = 0;
    viewportInfo.Width = currentExtent.width;
    viewportInfo.Height = currentExtent.height;
    viewportInfo.MinDepth = 0;
    viewportInfo.MaxDepth = 1;

    // Start command buffer recording
    currentCommandBuffer.BeginCommandBuffer();

    // Begin render pass
    currentCommandBuffer.BeginRenderPass(
        _framebuffers->GetFramebuffer(imageIndex).GetSwapChainFramebuffer(),
        _graphicsPipeline->GetGraphicsPipeline(),
        viewportInfo
    );
    auto descriptors = std::vector<VkDescriptorSet>{ _descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId()) };

    // Draw
    currentCommandBuffer.DrawIndexed(vertexBuffer, indexBuffer, descriptors);

    // End render pass
    currentCommandBuffer.EndRenderPass();

    // End command buffer recording
    currentCommandBuffer.EndCommandBuffer();

    // Submit command buffer to graphics queue
    VkCommandBuffer commandBuffers[] = { currentCommandBuffer.GetVkCommandBuffer() };
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = commandBuffers;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]->GetSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]->GetSemaphore() };
    if (_isOffscreen)
    {
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
    }
    else
    {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
    }

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _fences[_currentFrame]->GetFence()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    if (_isOffscreen)
    {
        // Update current frame
        _currentFrame = (_currentFrame + 1) % _maxFramesInFlight;
        return;
    }

    // Submit rendered image to presentation queue
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _swapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    auto presentImageResult = vkQueuePresentKHR(_logicalDevice.GetPresentationQueue(), &presentInfo);
    if (presentImageResult == VK_ERROR_OUT_OF_DATE_KHR || presentImageResult == VK_SUBOPTIMAL_KHR)
    {
        HandleFramebuffersResized();
    }
    else
    {
        M_ASSERT(presentImageResult == VK_SUCCESS && "Failed to acquire present swap chain image");
    }

	// Update current frame
	_currentFrame = (_currentFrame + 1) % _maxFramesInFlight;
}

void Mango::VulkanRenderer::HandleFramebuffersResized()
{
    M_TRACE("Swap chain is out of date. Recreating.");
    //vkDeviceWaitIdle(vkLogicalDevice);
    // Finish all rendering before recrating swap chain
    for (const auto& fence : _fences)
    {
        fence->WaitForFence();
    }

    auto vkPhysicalDevice = _hardwareInfo.PhysicalDevice.GetDevice();
    auto vkRenderSurface = _renderSurface.GetRenderSurface();
    auto queueFamilyIndices = _hardwareInfo.QueueFamilyIndices;
    auto swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(vkPhysicalDevice, vkRenderSurface);

    _swapChain.RecreateSwapChain(swapChainSupportDetails, queueFamilyIndices);
    _renderPass->RecreateRenderPass(_logicalDevice, _swapChain);

    const auto& imageViews = _swapChain.GetSwapChainImageViews();
    M_ASSERT(imageViews.size() == _framebuffers->GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        _framebuffers->GetFramebuffer(i).RecreateFramebuffer(*_renderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    }
}

void Mango::VulkanRenderer::UpdateUniformBuffer(uint32_t currentFrame)
{
    const auto& currentExtent = _swapChain.GetSwapChainExtent();

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    Mango::UniformBufferObject ubo{};
    ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.Projection = glm::perspective(glm::radians(45.0f), currentExtent.width / static_cast<float>(currentExtent.height), 0.1f, 10.0f);
    ubo.Projection[1][1] *= -1;

    const auto& uniformBuffer = _uniformBuffers->GetUniformBuffer(currentFrame);
    void* gpuMemory = uniformBuffer.MapMemory();
    memcpy(gpuMemory, &ubo, sizeof(ubo));
    uniformBuffer.UnmapMemory();
}
