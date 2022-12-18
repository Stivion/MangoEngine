#include "Application.h"
#include "Infrastructure/Logging/Logging.h"

#include "Vulkan/SwapChainSupportDetails.h"

Mango::Application::Application()
{
    CreateSyncObjects();
    InitializeImGui();
}

Mango::Application::~Application()
{
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    vkDestroySemaphore(vkLogicalDevice, _imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkLogicalDevice, _renderFinishedSemaphore, nullptr);
    vkDestroyFence(vkLogicalDevice, _inFlightFence, nullptr);


}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(_window.GetWindow()))
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        
        // Rendering
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized)
        {
            FrameRender(draw_data);
        }
    }

    vkDeviceWaitIdle(_logicalDevice.GetDevice());
}

void Mango::Application::FrameRender(ImDrawData* draw_data)
{
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    vkWaitForFences(vkLogicalDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkLogicalDevice, 1, &_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vkLogicalDevice, _swapChain.GetSwapChain(), UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    vkResetCommandBuffer(_commandBuffer.GetCommandBuffer(), 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(_commandBuffer.GetCommandBuffer(), &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    const auto& swapChainExtent = _swapChain.GetSwapChainExtent();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass.GetRenderPass();
    renderPassInfo.framebuffer = _framebuffers.GetSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(_commandBuffer.GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(_commandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline.GetGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_commandBuffer.GetCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(_commandBuffer.GetCommandBuffer(), 0, 1, &scissor);

    ImGui_ImplVulkan_RenderDrawData(draw_data, _commandBuffer.GetCommandBuffer());

    vkCmdEndRenderPass(_commandBuffer.GetCommandBuffer());

    if (vkEndCommandBuffer(_commandBuffer.GetCommandBuffer()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer.GetCommandBuffer();

    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
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

    vkQueuePresentKHR(_logicalDevice.GetPresentationQueue(), &presentInfo);
}

void Mango::Application::DrawFrame()
{
    const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    vkWaitForFences(vkLogicalDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkLogicalDevice, 1, &_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vkLogicalDevice, _swapChain.GetSwapChain(), UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    vkResetCommandBuffer(_commandBuffer.GetCommandBuffer(), 0);
    _commandBuffer.RecordCommandBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer.GetCommandBuffer();

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain.GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(_logicalDevice.GetPresentationQueue(), &presentInfo);
}

void Mango::Application::InitializeImGui()
{
    auto& vkPhysicalDevice = _physicalDevice.GetDevice();
    auto& vkLogicalDevice = _logicalDevice.GetDevice();
    auto& vkRenderSurface = _renderSurface.GetRenderSurface();

    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    std::cout << "Creating descriptor pool" << std::endl;
    if (vkCreateDescriptorPool(vkLogicalDevice, &pool_info, nullptr, &_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    const auto& imageFormats = _swapChain.GetSwapChainSurfaceFormat().format;
    std::cout << "Setting ImGui window" << std::endl;

    std::cout << "Creating ImGui context" << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = _window.GetWindowWidth();
    io.DisplaySize.y = _window.GetWindowHeight();
    ImGui::StyleColorsDark();

    std::cout << "Initializing ImGui for GLFW" << std::endl;
    ImGui_ImplGlfw_InitForVulkan(_window.GetWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = _instance.GetInstance();
    initInfo.PhysicalDevice = vkPhysicalDevice;
    initInfo.Device = vkLogicalDevice;
    initInfo.QueueFamily = _queueFamilyIndices.GraphicsFamily.value();
    initInfo.Queue = _logicalDevice.GetGraphicsQueue();
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = _descriptorPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = _swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    initInfo.ImageCount = _swapChain.GetSwapChainImages().size();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = nullptr;
    std::cout << "Initializing ImGui for Vulkan" << std::endl;
    ImGui_ImplVulkan_Init(&initInfo, _renderPass.GetRenderPass());

    // Use any command queue
    VkCommandPool command_pool = _commandPool.GetCommandPool();
    VkCommandBuffer command_buffer = _commandBuffer.GetCommandBuffer();

    if (vkResetCommandPool(vkLogicalDevice, command_pool, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command pool (fonts)");
    }
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin command buffer (fonts)");
    }

    std::cout << "Creating fonts" << std::endl;
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &command_buffer;
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer (fonts)");
    }
    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit graphics queue (fonts)");
    }

    vkDeviceWaitIdle(vkLogicalDevice);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Mango::Application::CreateSyncObjects()
{
    auto& vkLogicalDevice = _logicalDevice.GetDevice();
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vkLogicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(vkLogicalDevice, &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create sync objects");
    }
}
