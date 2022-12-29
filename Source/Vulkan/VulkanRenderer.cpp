#include "VulkanRenderer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

Mango::VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo createInfo)
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

    InitializeViewportMembers();
    InitializeUIMembers(createInfo.WindowFramebufferWidth, createInfo.WindowFramebufferHeight, swapChainSupportDetails);

    // TODO: Prepare vertex and index buffers (temporary)
    const uint32_t vertexCount = static_cast<uint32_t>(_vertices.size());
    const uint32_t indicesCount = static_cast<uint32_t>(_indices.size());
    _vertexBuffer = std::make_unique<Mango::VertexBuffer>(vertexCount, sizeof(Vertex) * vertexCount, _vertices.data(), *_physicalDevice, *_logicalDevice, *_commandPool);
    _indexBuffer = std::make_unique<Mango::IndexBuffer>(indicesCount, sizeof(uint16_t) * indicesCount, _indices.data(), *_physicalDevice, *_logicalDevice, *_commandPool);

    std::vector<VkDescriptorSet> descriptors;
    descriptors.push_back(_descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId()));

    const auto& currentExtent = _swapChain->GetSwapChainExtent();
    {
        Mango::ViewportInfo viewportInfo{};
        viewportInfo.X = 0;
        viewportInfo.Y = 0;
        viewportInfo.Width = currentExtent.width;
        viewportInfo.Height = currentExtent.height;
        viewportInfo.MinDepth = 0;
        viewportInfo.MaxDepth = 1;

        ViewportCommandBufferRecorderInfo info{};
        info.DescriptorPool = _descriptorPool.get();
        info.DescriptorSets = descriptors;
        info.GraphicsPipeline = _graphicsPipeline.get();
        info.IndexBuffer = _indexBuffer.get();
        info.VertexBuffer = _vertexBuffer.get();
        info.ViewportInfo = viewportInfo;
        _viewportCommandBufferRecorder = std::make_unique<Mango::ViewportCommandBufferRecorder>(info);
    }

    {
        Mango::ViewportInfo viewportInfo{};
        viewportInfo.X = 0;
        viewportInfo.Y = 0;
        viewportInfo.Width = currentExtent.width;
        viewportInfo.Height = currentExtent.height;
        viewportInfo.MinDepth = 0;
        viewportInfo.MaxDepth = 1;

        UICommandBufferRecorderInfo info{};
        info.LogicalDevice = _logicalDevice;
        info.PhysicalDevice = _physicalDevice;
        info.GraphicsPipeline = _graphicsPipeline.get();
        info.SwapChain = _swapChain.get();
        info.ViewportImage = _editorViewport->GetViewportImage();
        info.ViewportInfo = viewportInfo;
        _uiCommandBufferRecorder = std::make_unique<Mango::UICommandBufferRecorder>(info);
    }
}

Mango::VulkanRenderer::~VulkanRenderer()
{
    ImGui_ImplVulkan_Shutdown(); // TODO: This executes after application descturctor is called
}

void Mango::VulkanRenderer::Draw()
{
    this->Draw(*_vertexBuffer, *_indexBuffer);
}

void Mango::VulkanRenderer::Draw(Mango::VertexBuffer& vertexBuffer, Mango::IndexBuffer& indexBuffer)
{
	const auto& vkLogicalDevice = _logicalDevice->GetDevice();
    const auto& vkSwapChain = _swapChain->GetSwapChain();

    _fences[_currentFrame]->WaitForFence();
    
    // Acquire next image to render to
    const auto nextImageResult = _swapChain->AcquireNextImage(*_imageAvailableSemaphores[_currentFrame]);
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        // TODO: Handle minimized window
        HandleFramebuffersResized();
        return;
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    const auto imageIndex = _swapChain->GetCurrentImageIndex();
    _fences[_currentFrame]->ResetFence();

    UpdateUniformBuffer(_currentFrame);

    const auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);
    const auto& currentFramebuffer = _framebuffers->GetFramebuffer(imageIndex);

    const auto& currentImGuiCommandBuffer = _imGuiCommandBuffers->GetCommandBuffer(_currentFrame);
    const auto& currentImGuiFramebuffer = _imGuiFramebuffers->GetFramebuffer(imageIndex);

    _viewportCommandBufferRecorder->RecordCommandBuffer(&currentCommandBuffer, &currentFramebuffer);
    _uiCommandBufferRecorder->RecordCommandBuffer(&currentImGuiCommandBuffer, &currentImGuiFramebuffer);

    // Submit command buffer to graphics queue
    VkCommandBuffer commandBuffers[] = { currentCommandBuffer.GetVkCommandBuffer(), currentImGuiCommandBuffer.GetVkCommandBuffer() };
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = commandBuffers;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]->GetSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 2;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]->GetSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice->GetGraphicsQueue(), 1, &submitInfo, _fences[_currentFrame]->GetFence()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

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

    auto presentImageResult = vkQueuePresentKHR(_logicalDevice->GetPresentationQueue(), &presentInfo);
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

    auto vkPhysicalDevice = _physicalDevice->GetDevice();
    auto vkRenderSurface = _renderSurface->GetRenderSurface();
    auto swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(vkPhysicalDevice, vkRenderSurface);

    _swapChain->RecreateSwapChain(800, 600, swapChainSupportDetails, *_queueFamilyIndices); // TODO: Fix me
    _renderPass->RecreateRenderPass(*_logicalDevice, *_swapChain);

    const auto& imageViews = _swapChain->GetSwapChainImageViews();
    M_ASSERT(imageViews.size() == _framebuffers->GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        _framebuffers->GetFramebuffer(i).RecreateFramebuffer(*_renderPass, _swapChain->GetSwapChainExtent(), imageViews[i]);
    }
}

void Mango::VulkanRenderer::UpdateUniformBuffer(uint32_t currentFrame)
{
    const auto& currentExtent = _swapChain->GetSwapChainExtent();

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

void Mango::VulkanRenderer::InitializeViewportMembers()
{
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
        uint64_t bufferId = _uniformBuffers->CreateBuffer(createInfo);
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
}

void Mango::VulkanRenderer::InitializeUIMembers(
    uint32_t windowFramebufferWidth,
    uint32_t windowFramebufferHeight,
    Mango::SwapChainSupportDetails swapChainSupportDetails
)
{
    Mango::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    _imGuiRenderPass = std::make_unique<Mango::RenderPass>(*_logicalDevice, *_swapChain, renderPassCreateInfo);
    _imGuiFramebuffers = std::make_unique<Mango::FramebuffersPool>(*_logicalDevice, *_imGuiRenderPass, *_swapChain);
    _imGuiDescriptorPool = std::make_unique<Mango::DescriptorPool>(_imGuiPoolSizes, *_logicalDevice);
    _imGuiCommandPool = std::make_unique<Mango::CommandPool>(*_logicalDevice, *_queueFamilyIndices);
    _imGuiCommandBuffers = std::make_unique<Mango::CommandBuffersPool>(
        _maxFramesInFlight,
        *_logicalDevice,
        *_swapChain,
        *_imGuiRenderPass,
        *_imGuiCommandPool,
        *_graphicsPipeline
    );

    const auto& vkInstance = _instance->GetInstance();
    const auto& vkPhysicalDevice = _physicalDevice->GetDevice();
    const auto& vkLogicalDevice = _logicalDevice->GetDevice();
    const auto& vkGraphicsQueueFamily = _queueFamilyIndices->GraphicsFamily.value();
    const auto& vkGraphicsQueue = _logicalDevice->GetGraphicsQueue();
    const auto& vkDescriptorPool = _imGuiDescriptorPool->GetDescriptorPool();
    const auto& vkMinImageCount = swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    const auto& vkImageCount = static_cast<uint32_t>(_swapChain->GetSwapChainImages().size());
    const auto& vkRenderPass = _imGuiRenderPass->GetRenderPass();
    const auto& vkCommandPool = _imGuiCommandPool->GetCommandPool();
    const auto& vkCommandBuffer = _imGuiCommandBuffers->GetCommandBuffer(0).GetVkCommandBuffer();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = vkInstance;
    initInfo.PhysicalDevice = vkPhysicalDevice;
    initInfo.Device = vkLogicalDevice;
    initInfo.QueueFamily = vkGraphicsQueueFamily;
    initInfo.Queue = vkGraphicsQueue;
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = vkDescriptorPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = vkMinImageCount;
    initInfo.ImageCount = vkImageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = CheckImGuiVulkanResultFn;
    ImGui_ImplVulkan_Init(&initInfo, vkRenderPass);

    // Render fonts
    auto resetCommandPoolResult = vkResetCommandPool(vkLogicalDevice, vkCommandPool, 0);
    M_TRACE("Reset command pool result is: " + std::to_string(resetCommandPoolResult));
    M_ASSERT(resetCommandPoolResult == VK_SUCCESS && "Failed to reset command pool");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    auto beginCommandBufferResult = vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
    M_TRACE("Begin command buffer result is: " + std::to_string(beginCommandBufferResult));
    M_ASSERT(beginCommandBufferResult == VK_SUCCESS && "Failed to begin command buffer");

    ImGui_ImplVulkan_CreateFontsTexture(vkCommandBuffer);

    VkSubmitInfo endInfo = {};
    endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    endInfo.commandBufferCount = 1;
    endInfo.pCommandBuffers = &vkCommandBuffer;
    auto endCommandBufferResult = vkEndCommandBuffer(vkCommandBuffer);
    M_TRACE("End commnad buffer result is: " + std::to_string(endCommandBufferResult));
    M_ASSERT(endCommandBufferResult == VK_SUCCESS && "Failed to end command buffer");

    auto queueSubmitResult = vkQueueSubmit(vkGraphicsQueue, 1, &endInfo, VK_NULL_HANDLE);
    M_TRACE("Queue submit result is: " + std::to_string(queueSubmitResult));
    M_ASSERT(queueSubmitResult == VK_SUCCESS && "Failed to submit fonts rendering to queue");

    vkDeviceWaitIdle(vkLogicalDevice);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
    // End fonts rendering

    Mango::EditorViewportCreateInfo editorViewportCreateInfo{};
    editorViewportCreateInfo.PhysicalDevice = _physicalDevice;
    editorViewportCreateInfo.LogicalDevice = _logicalDevice;
    editorViewportCreateInfo.SwapChain = _swapChain.get();
    _editorViewport = std::make_unique<Mango::EditorViewport>(editorViewportCreateInfo);
}

void Mango::VulkanRenderer::CheckImGuiVulkanResultFn(VkResult result)
{
    M_ASSERT(result == VK_SUCCESS && "Error inside ImGui");
}
