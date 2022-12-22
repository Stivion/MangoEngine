#include "Application.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"
#include "Vulkan/SwapChainSupportDetails.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();
    InitializeImGui();
}

Mango::Application::~Application()
{
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    for (auto i = 0; i < MaxFramesInFlight; i++)
    {
        vkDestroySemaphore(vkLogicalDevice, _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(vkLogicalDevice, _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(vkLogicalDevice, _inFlightFences[i], nullptr);
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::InitializeWindow()
{
    _window.SetFramebufferResizedCallback(FramebufferResizedCallback);
    _window.SetWindowUserPointer(this);
}

void Mango::Application::InitializeVulkan()
{
    CreateSyncObjects();
}

void Mango::Application::InitializeImGui()
{
    const auto& vkInstance = _instance.GetInstance();
    const auto& vkPhysicalDevice = _physicalDevice.GetDevice();
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    const auto& vkGraphicsQueueFamily = _queueFamilyIndices.GraphicsFamily.value();
    const auto& vkGraphicsQueue = _logicalDevice.GetGraphicsQueue();
    const auto& vkDescriptorPool = _descriptorPool.GetDescriptorPool();
    const auto& vkMinImageCount = _swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    const auto& vkImageCount = static_cast<uint32_t>(_swapChain.GetSwapChainImages().size());
    const auto& vkRenderPass = _renderPass.GetRenderPass();
    const auto& vkCommandPool = _commandPool.GetCommandPool();
    const auto& vkCommandBuffer = _commandBuffers.GetCommandBuffer(0).GetVkCommandBuffer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(_window.GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(_window.GetWindowHeight());
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(_window.GetWindow(), true);
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
    // End render fonts
}

void Mango::Application::CreateSyncObjects()
{
    auto& vkLogicalDevice = _logicalDevice.GetDevice();
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto i = 0; i < MaxFramesInFlight; i++)
    {
        if (vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vkLogicalDevice, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create sync objects");
        }
    }
}

void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    uint32_t currentFrame = 0;
    const std::vector<Vertex> vertices =
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},

        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{1.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}}
    };
    const std::vector<uint16_t> indices =
    {
        0, 1, 2, 3, 4, 5, 5, 6, 3
    };
    const auto vertexCount = static_cast<uint32_t>(vertices.size());
    const auto indicesCount = static_cast<uint32_t>(indices.size());
    const Mango::VertexBuffer vertexBuffer(vertexCount, sizeof(Vertex) * vertexCount, vertices.data(), _physicalDevice, _logicalDevice, _commandPool);
    const Mango::IndexBuffer indexBuffer(indicesCount, sizeof(uint16_t) * indicesCount, indices.data(), _physicalDevice, _logicalDevice, _commandPool);

    while (!glfwWindowShouldClose(_window.GetWindow()))
    {
        glfwPollEvents();
        DrawFrame(currentFrame, vertexBuffer, indexBuffer);
        currentFrame = (currentFrame + 1) % MaxFramesInFlight;
    }

    vkDeviceWaitIdle(_logicalDevice.GetDevice());
}

void Mango::Application::DrawFrame(uint32_t currentFrame, const Mango::VertexBuffer& vertexBuffer, const Mango::IndexBuffer& indexBuffer)
{
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();

    vkWaitForFences(vkLogicalDevice, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto nextImageResult = vkAcquireNextImageKHR(
        vkLogicalDevice,
        _swapChain.GetSwapChain(),
        UINT64_MAX,
        _imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
        if (width == 0 || height == 0)
        {
            M_TRACE("Window is minimized.");
        }
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
            glfwWaitEvents();
        }

        M_TRACE("Swap chain is out of date. Recreating.");
        vkDeviceWaitIdle(vkLogicalDevice);

        _swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        _swapChain.RecreateSwapChain(_swapChainSupportDetails, _queueFamilyIndices);
        _renderPass.RecreateRenderPass(_logicalDevice, _swapChain);

        // TODO: Extract into method
        const auto& imageViews = _swapChain.GetSwapChainImageViews();
        M_ASSERT(imageViews.size() == _framebuffers.GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            _framebuffers.GetFramebuffer(i).RecreateFramebuffer(_renderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
        }
        return;
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    // Reset only if we would issue draw calls
    vkResetFences(vkLogicalDevice, 1, &_inFlightFences[currentFrame]);

    auto& currentCommandBuffer = _commandBuffers.GetCommandBuffer(currentFrame);
    vkResetCommandBuffer(currentCommandBuffer.GetVkCommandBuffer(), 0);
    vkDeviceWaitIdle(_logicalDevice.GetDevice()); // FIX ME
    UpdateUniformBuffer(currentFrame);

    // Begin drawing
    currentCommandBuffer.BeginCommandBuffer(_framebuffers.GetFramebuffer(imageIndex).GetSwapChainFramebuffer());

    // Render ImGui interface
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    ImGui::Begin("Viewport");
    //const auto& currentDescriptorSet = _descriptorPool.GetDescriptorSets()[currentFrame];
    //ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    //const glm::vec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    //ImGui::Image(currentDescriptorSet, ImVec2{ viewportSize.x, viewportSize.y });
    ImGui::End();

    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    //ImGui_ImplVulkan_RenderDrawData(drawData, currentCommandBuffer.GetVkCommandBuffer());
    
    ImGui::EndFrame();

    // Render some custom graphics
    currentCommandBuffer.DrawIndexed(vertexBuffer, indexBuffer, _uniformBuffers.GetDescriptorSet(currentFrame));

    // End drawing
    currentCommandBuffer.EndCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pCommandBuffers = &currentCommandBuffer.GetVkCommandBuffer();

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

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
    if (presentImageResult == VK_ERROR_OUT_OF_DATE_KHR || presentImageResult == VK_SUBOPTIMAL_KHR || _framebufferResized)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
        if (width == 0 || height == 0)
        {
            M_TRACE("Window is minimized.");
        }
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
            glfwWaitEvents();
        }

        M_TRACE("Swap chain is out of date. Recreating.");
        _framebufferResized = false;
        vkDeviceWaitIdle(vkLogicalDevice);

        _swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        _swapChain.RecreateSwapChain(_swapChainSupportDetails, _queueFamilyIndices);
        _renderPass.RecreateRenderPass(_logicalDevice, _swapChain);

        // TODO: Extract into method
        const auto& imageViews = _swapChain.GetSwapChainImageViews();
        M_ASSERT(imageViews.size() == _framebuffers.GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            _framebuffers.GetFramebuffer(i).RecreateFramebuffer(_renderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
        }
    }
    else
    {
        M_ASSERT(presentImageResult == VK_SUCCESS && "Failed to acquire present swap chain image");
    }
}

void Mango::Application::UpdateUniformBuffer(uint32_t currentFrame)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    Mango::UniformBufferObject ubo{};
    ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.Projection = glm::perspective(glm::radians(45.0f), _swapChainSupportDetails.SurfaceCapabilities.currentExtent.width / static_cast<float>(_swapChainSupportDetails.SurfaceCapabilities.currentExtent.width), 0.1f, 10.0f);
    ubo.Projection[1][1] *= -1;
    memcpy(_uniformBuffers.GetUniformBuffer(currentFrame).GetMappedMemoryPointer(), &ubo, sizeof(ubo));
}

void Mango::Application::FramebufferResizedCallback(GLFWwindow* window, int width, int height)
{
    auto applicationPointer = reinterpret_cast<Mango::Application*>(glfwGetWindowUserPointer(window));
    applicationPointer->_framebufferResized = true;
}

void Mango::Application::CheckImGuiVulkanResultFn(VkResult result)
{
    M_ASSERT(result == VK_SUCCESS && "Error inside ImGui");
}
