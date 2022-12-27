#include "Application.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"
#include "Vulkan/SwapChainSupportDetails.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <fstream>

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();
    InitializeImGui();
}

Mango::Application::~Application()
{
    //ImGui_ImplVulkan_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
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
}

void Mango::Application::InitializeImGui()
{
    //const auto& vkInstance = _instance.GetInstance();
    //const auto& vkPhysicalDevice = _physicalDevice.GetDevice();
    //const auto& vkLogicalDevice = _logicalDevice.GetDevice();
    //const auto& vkGraphicsQueueFamily = _queueFamilyIndices.GraphicsFamily.value();
    //const auto& vkGraphicsQueue = _logicalDevice.GetGraphicsQueue();
    //const auto& vkDescriptorPool = _imGuiDescriptorPool.GetDescriptorPool();
    //const auto& vkMinImageCount = _swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    //const auto& vkImageCount = static_cast<uint32_t>(_swapChain.GetSwapChainImages().size());
    //const auto& vkRenderPass = _imGuiRenderPass.GetRenderPass();
    //const auto& vkCommandPool = _imGuiCommandPool.GetCommandPool();
    //const auto& vkCommandBuffer = _imGuiCommandBuffers.GetCommandBuffer(0).GetVkCommandBuffer();

    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize.x = static_cast<float>(_window.GetWindowWidth());
    //io.DisplaySize.y = static_cast<float>(_window.GetWindowHeight());
    //ImGui::StyleColorsDark();

    //ImGui_ImplGlfw_InitForVulkan(_window.GetWindow(), true);
    //ImGui_ImplVulkan_InitInfo initInfo{};
    //initInfo.Instance = vkInstance;
    //initInfo.PhysicalDevice = vkPhysicalDevice;
    //initInfo.Device = vkLogicalDevice;
    //initInfo.QueueFamily = vkGraphicsQueueFamily;
    //initInfo.Queue = vkGraphicsQueue;
    //initInfo.PipelineCache = nullptr;
    //initInfo.DescriptorPool = vkDescriptorPool;
    //initInfo.Subpass = 0;
    //initInfo.MinImageCount = vkMinImageCount;
    //initInfo.ImageCount = vkImageCount;
    //initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    //initInfo.Allocator = nullptr;
    //initInfo.CheckVkResultFn = CheckImGuiVulkanResultFn;
    //ImGui_ImplVulkan_Init(&initInfo, vkRenderPass);

    //// Render fonts
    //auto resetCommandPoolResult = vkResetCommandPool(vkLogicalDevice, vkCommandPool, 0);
    //M_TRACE("Reset command pool result is: " + std::to_string(resetCommandPoolResult));
    //M_ASSERT(resetCommandPoolResult == VK_SUCCESS && "Failed to reset command pool");

    //VkCommandBufferBeginInfo beginInfo = {};
    //beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    //auto beginCommandBufferResult = vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
    //M_TRACE("Begin command buffer result is: " + std::to_string(beginCommandBufferResult));
    //M_ASSERT(beginCommandBufferResult == VK_SUCCESS && "Failed to begin command buffer");

    //ImGui_ImplVulkan_CreateFontsTexture(vkCommandBuffer);

    //VkSubmitInfo endInfo = {};
    //endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //endInfo.commandBufferCount = 1;
    //endInfo.pCommandBuffers = &vkCommandBuffer;
    //auto endCommandBufferResult = vkEndCommandBuffer(vkCommandBuffer);
    //M_TRACE("End commnad buffer result is: " + std::to_string(endCommandBufferResult));
    //M_ASSERT(endCommandBufferResult == VK_SUCCESS && "Failed to end command buffer");

    //auto queueSubmitResult = vkQueueSubmit(vkGraphicsQueue, 1, &endInfo, VK_NULL_HANDLE);
    //M_TRACE("Queue submit result is: " + std::to_string(queueSubmitResult));
    //M_ASSERT(queueSubmitResult == VK_SUCCESS && "Failed to submit fonts rendering to queue");

    //vkDeviceWaitIdle(vkLogicalDevice);
    //ImGui_ImplVulkan_DestroyFontUploadObjects();
    //// End fonts rendering
}

void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    uint32_t currentFrame = 0;

    //const auto& currentExtent = _swapChain.GetSwapChainExtent();
    //// Create the Vulkan image.
    //VkImage image;
    //VkDeviceMemory imageMemory;
    //{
    //    VkImageCreateInfo info = {};
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    //    info.imageType = VK_IMAGE_TYPE_2D;
    //    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    //    info.extent.width = currentExtent.width;
    //    info.extent.height = currentExtent.height;
    //    info.extent.depth = 1;
    //    info.mipLevels = 1;
    //    info.arrayLayers = 1;
    //    info.samples = VK_SAMPLE_COUNT_1_BIT;
    //    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    //    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    //    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    vkCreateImage(_logicalDevice.GetDevice(), &info, nullptr, &image);
    //    VkMemoryRequirements req;
    //    vkGetImageMemoryRequirements(_logicalDevice.GetDevice(), image, &req);
    //    VkMemoryAllocateInfo alloc_info = {};
    //    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //    alloc_info.allocationSize = req.size;
    //    alloc_info.memoryTypeIndex = _physicalDevice.FindSuitableMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //    vkAllocateMemory(_logicalDevice.GetDevice(), &alloc_info, nullptr, &imageMemory);
    //    vkBindImageMemory(_logicalDevice.GetDevice(), image, imageMemory, 0);
    //}

    //VkImageMemoryBarrier barrier{};
    //barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //barrier.image = image;
    //barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //barrier.subresourceRange.baseMipLevel = 0;
    //barrier.subresourceRange.levelCount = 1;
    //barrier.subresourceRange.baseArrayLayer = 0;
    //barrier.subresourceRange.layerCount = 1;
    //barrier.srcAccessMask = 0; // TODO
    //barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; // TODO

    //VkImageMemoryBarrier secondBarrier{};
    //secondBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //secondBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //secondBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //secondBarrier.image = image;
    //secondBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //secondBarrier.subresourceRange.baseMipLevel = 0;
    //secondBarrier.subresourceRange.levelCount = 1;
    //secondBarrier.subresourceRange.baseArrayLayer = 0;
    //secondBarrier.subresourceRange.layerCount = 1;
    //secondBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; // TODO
    //secondBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; // TODO

    //// Create the Image View
    //VkImageView imageView;
    //{
    //    VkImageViewCreateInfo info = {};
    //    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //    info.image = image;
    //    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    //    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    //    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //    info.subresourceRange.levelCount = 1;
    //    info.subresourceRange.layerCount = 1;
    //    vkCreateImageView(_logicalDevice.GetDevice(), &info, nullptr, &imageView);
    //}

    //// Create Sampler
    //VkSampler sampler;
    //{
    //    VkSamplerCreateInfo sampler_info{};
    //    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //    sampler_info.magFilter = VK_FILTER_LINEAR;
    //    sampler_info.minFilter = VK_FILTER_LINEAR;
    //    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // outside image bounds just use border color
    //    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //    sampler_info.minLod = -1000;
    //    sampler_info.maxLod = 1000;
    //    sampler_info.maxAnisotropy = 1.0f;
    //    vkCreateSampler(_logicalDevice.GetDevice(), &sampler_info, nullptr, &sampler);
    //}

    //// Create Descriptor Set using ImGUI's implementation
    //VkDescriptorSet renderedImageDescriptorSet = ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    while (!glfwWindowShouldClose(_window.GetWindow()))
    {
        glfwPollEvents();
        DrawFrame(currentFrame);
        currentFrame = (currentFrame + 1) % MaxFramesInFlight;
    }

    vkDeviceWaitIdle(_logicalDevice.GetDevice());
}

void Mango::Application::DrawFrame(uint32_t currentFrame)
{
    _renderer->Draw();
    //const auto& vkLogicalDevice = _logicalDevice.GetDevice();

    //vkWaitForFences(vkLogicalDevice, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //uint32_t imageIndex;
    //auto nextImageResult = vkAcquireNextImageKHR(
    //    vkLogicalDevice,
    //    _swapChain.GetSwapChain(),
    //    UINT64_MAX,
    //    _imageAvailableSemaphores[currentFrame],
    //    VK_NULL_HANDLE,
    //    &imageIndex
    //);
    //if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    //{
    //    int width = 0, height = 0;
    //    glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
    //    if (width == 0 || height == 0)
    //    {
    //        M_TRACE("Window is minimized.");
    //    }
    //    while (width == 0 || height == 0)
    //    {
    //        glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
    //        glfwWaitEvents();
    //    }

    //    M_TRACE("Swap chain is out of date. Recreating.");
    //    vkDeviceWaitIdle(vkLogicalDevice);

    //    _swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
    //    _swapChain.RecreateSwapChain(_swapChainSupportDetails, _queueFamilyIndices);
    //    _renderPass.RecreateRenderPass(_logicalDevice, _swapChain);
    //    _imGuiRenderPass.RecreateRenderPass(_logicalDevice, _swapChain);

    //    // TODO: Extract into method
    //    const auto& imageViews = _swapChain.GetSwapChainImageViews();
    //    M_ASSERT(imageViews.size() == _framebuffers.GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    //    M_ASSERT(imageViews.size() == _imGuiFramebuffers.GetFramebuffersCount() && "ImGui framebuffers count and image views count doesn't match");
    //    for (size_t i = 0; i < imageViews.size(); i++)
    //    {
    //        _framebuffers.GetFramebuffer(i).RecreateFramebuffer(_renderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    //        _imGuiFramebuffers.GetFramebuffer(i).RecreateFramebuffer(_imGuiRenderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    //    }
    //    return;
    //}
    //else
    //{
    //    M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    //}

    //// Reset only if we would issue draw calls
    //vkResetFences(vkLogicalDevice, 1, &_inFlightFences[currentFrame]);

    //auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(currentFrame);
    //vkResetCommandBuffer(currentCommandBuffer.GetVkCommandBuffer(), 0);
    //UpdateUniformBuffer(currentFrame);

    //// Begin recording command buffer
    //currentCommandBuffer.BeginCommandBuffer();

    //// Render some custom graphics
    //const auto& currentExtent = _swapChain.GetSwapChainExtent();
    //Mango::ViewportInfo viewportInfo{};
    //viewportInfo.X = 0;
    //viewportInfo.Y = 0;
    //viewportInfo.Width = currentExtent.width;
    //viewportInfo.Height = currentExtent.height;
    //viewportInfo.MinDepth = 0;
    //viewportInfo.MaxDepth = 1;

    //// Begin drawing
    //currentCommandBuffer.BeginRenderPass(
    //    _framebuffers.GetFramebuffer(currentFrame).GetSwapChainFramebuffer(),
    //    _graphicsPipeline->GetGraphicsPipeline(),
    //    viewportInfo
    //);
    //auto descriptors = std::vector<VkDescriptorSet>{ _descriptorPool.GetDescriptorSet(_globalLayout->GetId()) };

    //currentCommandBuffer.DrawIndexed(vertexBuffer, indexBuffer, descriptors);

    //// End drawing
    //currentCommandBuffer.EndRenderPass();

    //// End recording command buffer
    //currentCommandBuffer.EndCommandBuffer();


    //// Copy rendered image & draw UI
    //auto& imGuiCommandBuffer = _imGuiCommandBuffers.GetCommandBuffer(currentFrame);
    //vkResetCommandBuffer(imGuiCommandBuffer.GetVkCommandBuffer(), 0);
    //VkCommandBufferBeginInfo beginInfo{};
    //beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //beginInfo.flags = 0; // Optional
    //beginInfo.pInheritanceInfo = nullptr; // Optional

    //auto beginCommandBufferResult = vkBeginCommandBuffer(imGuiCommandBuffer.GetVkCommandBuffer(), &beginInfo);
    //M_ASSERT(beginCommandBufferResult == VK_SUCCESS && "Failed to begin recording command buffer");
    ////imGuiCommandBuffer.BeginCommandBuffer(_imGuiFramebuffers.GetFramebuffer(imageIndex).GetSwapChainFramebuffer());

    //vkCmdPipelineBarrier(imGuiCommandBuffer.GetVkCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    //const VkDeviceSize imageSize = currentExtent.width * currentExtent.height * 4;

    //VkImageSubresourceLayers srcSubResourceLayer{};
    //srcSubResourceLayer.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //srcSubResourceLayer.baseArrayLayer = 0;
    //srcSubResourceLayer.layerCount = 1;
    //srcSubResourceLayer.mipLevel = 0;

    //VkImageCopy region{};
    //region.srcOffset = { 0, 0, 0 };
    //region.dstOffset = { 0, 0, 0 };
    //region.extent.width = currentExtent.width;
    //region.extent.height = currentExtent.height;
    //region.extent.depth = 1;
    //region.srcSubresource = srcSubResourceLayer;
    //region.dstSubresource = srcSubResourceLayer;
    //vkCmdCopyImage(
    //    imGuiCommandBuffer.GetVkCommandBuffer(),
    //    _swapChain.GetSwapChainImages()[imageIndex],
    //    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    //    image,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    1,
    //    &region
    //);

    //vkCmdPipelineBarrier(imGuiCommandBuffer.GetVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &secondBarrier);

    //const auto& swapChainExtent = _swapChain.GetSwapChainExtent();
    //VkRenderPassBeginInfo renderPassInfo{};
    //renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //renderPassInfo.renderPass = _imGuiRenderPass.GetRenderPass();
    //renderPassInfo.framebuffer = _imGuiFramebuffers.GetFramebuffer(imageIndex).GetSwapChainFramebuffer();
    //renderPassInfo.renderArea.offset = { 0, 0 };
    //renderPassInfo.renderArea.extent = swapChainExtent;

    //VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    //renderPassInfo.clearValueCount = 1;
    //renderPassInfo.pClearValues = &clearColor;

    //vkCmdBeginRenderPass(imGuiCommandBuffer.GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //// Render ImGui interface
    //ImGui_ImplVulkan_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();

    //ImGui::ShowDemoWindow();
    //ImGui::Begin("Viewport");

    //ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    //const glm::vec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    //ImGui::Image(viewportDescriptorSet, ImVec2{ viewportSize.x, viewportSize.y });
    //ImGui::End();

    //ImGui::Render();
    //ImDrawData* drawData = ImGui::GetDrawData();
    //ImGui_ImplVulkan_RenderDrawData(drawData, imGuiCommandBuffer.GetVkCommandBuffer());
    //
    //ImGui::EndFrame();

    //vkCmdEndRenderPass(imGuiCommandBuffer.GetVkCommandBuffer()); // TODO: Fix Command buffers code
    //imGuiCommandBuffer.EndCommandBuffer();

    //VkCommandBuffer commandBuffers[] = { currentCommandBuffer.GetVkCommandBuffer(), imGuiCommandBuffer.GetVkCommandBuffer() };
    //VkSubmitInfo submitInfo{};
    //submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //submitInfo.pCommandBuffers = commandBuffers;

    //VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
    //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    //submitInfo.waitSemaphoreCount = 1;
    //submitInfo.pWaitSemaphores = waitSemaphores;
    //submitInfo.pWaitDstStageMask = waitStages;
    //submitInfo.commandBufferCount = 2;

    //VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };

    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = signalSemaphores;

    //if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS) 
    //{
    //    throw std::runtime_error("Failed to submit draw command buffer");
    //}

    //VkPresentInfoKHR presentInfo{};
    //presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    //presentInfo.waitSemaphoreCount = 1;
    //presentInfo.pWaitSemaphores = signalSemaphores;

    //VkSwapchainKHR swapChains[] = { _swapChain.GetSwapChain() };
    //presentInfo.swapchainCount = 1;
    //presentInfo.pSwapchains = swapChains;
    //presentInfo.pImageIndices = &imageIndex;
    //presentInfo.pResults = nullptr; // Optional

    //auto presentImageResult = vkQueuePresentKHR(_logicalDevice.GetPresentationQueue(), &presentInfo);
    //if (presentImageResult == VK_ERROR_OUT_OF_DATE_KHR || presentImageResult == VK_SUBOPTIMAL_KHR || _framebufferResized)
    //{
    //    int width = 0, height = 0;
    //    glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
    //    if (width == 0 || height == 0)
    //    {
    //        M_TRACE("Window is minimized.");
    //    }
    //    while (width == 0 || height == 0)
    //    {
    //        glfwGetFramebufferSize(_window.GetWindow(), &width, &height);
    //        glfwWaitEvents();
    //    }

    //    M_TRACE("Swap chain is out of date. Recreating.");
    //    _framebufferResized = false;
    //    vkDeviceWaitIdle(vkLogicalDevice);

    //    _swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
    //    _swapChain.RecreateSwapChain(_swapChainSupportDetails, _queueFamilyIndices);
    //    _renderPass.RecreateRenderPass(_logicalDevice, _swapChain);
    //    _imGuiRenderPass.RecreateRenderPass(_logicalDevice, _swapChain);

    //    // TODO: Extract into method
    //    const auto& imageViews = _swapChain.GetSwapChainImageViews();
    //    M_ASSERT(imageViews.size() == _framebuffers.GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    //    M_ASSERT(imageViews.size() == _imGuiFramebuffers.GetFramebuffersCount() && "ImGui framebuffers count and image views count doesn't match");
    //    for (size_t i = 0; i < imageViews.size(); i++)
    //    {
    //        _framebuffers.GetFramebuffer(i).RecreateFramebuffer(_renderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    //        _imGuiFramebuffers.GetFramebuffer(i).RecreateFramebuffer(_imGuiRenderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    //    }
    //}
    //else
    //{
    //    M_ASSERT(presentImageResult == VK_SUCCESS && "Failed to acquire present swap chain image");
    //}
}

void Mango::Application::FramebufferResizedCallback(GLFWwindow* window, int width, int height)
{
    auto applicationPointer = reinterpret_cast<Mango::Application*>(glfwGetWindowUserPointer(window));
    // TODO: Fix me
    //int width = 0, height = 0;
    //glfwGetFramebufferSize(window, &width, &height);
    //if (width == 0 || height == 0)
    //{
    //    M_TRACE("Window is minimized.");
    //}
    //while (width == 0 || height == 0)
    //{
    //    glfwGetFramebufferSize(window, &width, &height);
    //    glfwWaitEvents();
    //}
    //applicationPointer->_renderer->HandleFramebuffersResized();
    applicationPointer->_framebufferResized = true;
}

void Mango::Application::CheckImGuiVulkanResultFn(VkResult result)
{
    M_ASSERT(result == VK_SUCCESS && "Error inside ImGui");
}
