#include "ImGuiEditor_ImplGLFWVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::ImGuiEditor_ImplGLFWVulkan::ImGuiEditor_ImplGLFWVulkan(ImGuiEditor_ImplGLFWVulkan_CreateInfo createInfo)
    : ImGuiEditor(createInfo.Window), _logicalDevice(*createInfo.LogicalDevice), _swapChain(*createInfo.SwapChain), 
    _graphicsPipeline(*createInfo.GraphicsPipeline), _viewportInfo(createInfo.ViewportInfo), _maxFramesInFlight(createInfo.MaxFramesInFlight)
{
    const auto& instance = *createInfo.Instance;
    const auto& physicalDevice = *createInfo.PhysicalDevice;
    const auto& renderSurface = *createInfo.RenderSurface;
    const auto& logicalDevice = *createInfo.LogicalDevice;
    const auto& queueFamilyIndices = *createInfo.QueueFamilyIndices;
    Mango::SwapChainSupportDetails swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        physicalDevice.GetDevice(),
        renderSurface.GetRenderSurface()
    );

    Mango::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    _imGuiRenderPass = std::make_unique<Mango::RenderPass>(logicalDevice, _swapChain, renderPassCreateInfo);
    _imGuiFramebuffers = std::make_unique<Mango::FramebuffersPool>(logicalDevice, *_imGuiRenderPass, _swapChain);
    _imGuiDescriptorPool = std::make_unique<Mango::DescriptorPool>(_imGuiPoolSizes, logicalDevice, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    _imGuiCommandPool = std::make_unique<Mango::CommandPool>(logicalDevice, queueFamilyIndices);
    _imGuiCommandBuffers = std::make_unique<Mango::CommandBuffersPool>(
        _maxFramesInFlight,
        logicalDevice,
        _swapChain,
        *_imGuiRenderPass,
        *_imGuiCommandPool,
        _graphicsPipeline
    );

    const auto& vkInstance = instance.GetInstance();
    const auto& vkPhysicalDevice = physicalDevice.GetDevice();
    const auto& vkLogicalDevice = logicalDevice.GetDevice();
    const auto& vkGraphicsQueueFamily = queueFamilyIndices.GraphicsFamily.value();
    const auto& vkGraphicsQueue = logicalDevice.GetGraphicsQueue();
    const auto& vkDescriptorPool = _imGuiDescriptorPool->GetDescriptorPool();
    const auto& vkMinImageCount = swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    const auto& vkImageCount = static_cast<uint32_t>(_swapChain.GetSwapChainImages().size());
    const auto& vkRenderPass = _imGuiRenderPass->GetRenderPass();
    const auto& vkCommandPool = _imGuiCommandPool->GetCommandPool();
    const auto& vkCommandBuffer = _imGuiCommandBuffers->GetCommandBuffer(0).GetVkCommandBuffer();

    ImGui_ImplGlfw_InitForVulkan(createInfo.Window->GetGLFWWindow(), true);
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

    // Initialize editor viewport texture
    Mango::ImGuiEditorViewport_ImplVulkan_CreateInfo editorViewportCreateInfo{};
    editorViewportCreateInfo.PhysicalDevice = createInfo.PhysicalDevice;
    editorViewportCreateInfo.LogicalDevice = createInfo.LogicalDevice;
    editorViewportCreateInfo.SwapChain = createInfo.SwapChain;
    _imGuiEditorViewport = std::make_unique<Mango::ImGuiEditorViewport_ImplVulkan>(editorViewportCreateInfo);
    _viewportTextureId = _imGuiEditorViewport->GetViewportImageDescriptorSet();
    // End initialize editor viewport texture

    // Create image transition barriers
    _toDestinationTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_NONE,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );
    _toShaderReadTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT
    );
    // End create image transition barriers

    // Initialize sub resource layer
    _imageSubresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _imageSubresourceLayers.baseArrayLayer = 0;
    _imageSubresourceLayers.layerCount = 1;
    _imageSubresourceLayers.mipLevel = 0;
    // End initialize sub resource layer

    // Initialize copy region
    const auto& currentExtent = _swapChain.GetSwapChainExtent();
    _copyRegion.srcOffset = { 0, 0, 0 };
    _copyRegion.dstOffset = { 0, 0, 0 };
    _copyRegion.extent.width = currentExtent.width;
    _copyRegion.extent.height = currentExtent.height;
    _copyRegion.extent.depth = 1;
    _copyRegion.srcSubresource = _imageSubresourceLayers;
    _copyRegion.dstSubresource = _imageSubresourceLayers;
    // End initialize copy region
}

Mango::ImGuiEditor_ImplGLFWVulkan::~ImGuiEditor_ImplGLFWVulkan()
{
    // It is important to destory this viewport first, before we shutdown ImGui_ImplVulkan
    _imGuiEditorViewport = nullptr;
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::NewFrame(uint32_t currentFrame)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    Mango::ImGuiEditor::NewFrame(currentFrame);
    _currentFrame = currentFrame % _maxFramesInFlight;
}

void Mango::ImGuiEditor_ImplGLFWVulkan::EndFrame()
{
    Mango::ImGuiEditor::EndFrame();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::Draw() const
{
    const auto& currentImageIndex = _swapChain.GetCurrentImageIndex();
    const auto& currentCommandBuffer = _imGuiCommandBuffers->GetCommandBuffer(_currentFrame);
    const auto& currentFramebuffer = _imGuiFramebuffers->GetFramebuffer(currentImageIndex);

    const auto& currentVkCommandBuffer = currentCommandBuffer.GetVkCommandBuffer();

    currentCommandBuffer.Reset();
    currentCommandBuffer.BeginCommandBuffer();

    vkCmdPipelineBarrier(
        currentVkCommandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &_toDestinationTransitionBarrier
    );

    vkCmdCopyImage(
        currentVkCommandBuffer,
        _swapChain.GetCurrentSwapChainImage(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        _imGuiEditorViewport->GetViewportImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &_copyRegion
    );

    vkCmdPipelineBarrier(
        currentVkCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &_toShaderReadTransitionBarrier
    );

    currentCommandBuffer.BeginRenderPass(
        currentFramebuffer.GetSwapChainFramebuffer(),
        _graphicsPipeline.GetGraphicsPipeline(),
        _viewportInfo
    );

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentVkCommandBuffer);

    currentCommandBuffer.EndRenderPass();
    currentCommandBuffer.EndCommandBuffer();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::HandleResize()
{
    _imGuiRenderPass->RecreateRenderPass(_logicalDevice, _swapChain);
    const auto& imageViews = _swapChain.GetSwapChainImageViews();
    M_ASSERT(imageViews.size() == _imGuiFramebuffers->GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        _imGuiFramebuffers->GetFramebuffer(i).RecreateFramebuffer(*_imGuiRenderPass, _swapChain.GetSwapChainExtent(), imageViews[i]);
    }
    _imGuiEditorViewport->RecreateEditorViewport();
    _viewportTextureId = _imGuiEditorViewport->GetViewportImageDescriptorSet();

    _toDestinationTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_NONE,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );
    _toShaderReadTransitionBarrier = CreateImageMemoryBarrier(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT
    );

    const auto& currentExtent = _swapChain.GetSwapChainExtent();
    _copyRegion.srcOffset = { 0, 0, 0 };
    _copyRegion.dstOffset = { 0, 0, 0 };
    _copyRegion.extent.width = currentExtent.width;
    _copyRegion.extent.height = currentExtent.height;
    _copyRegion.extent.depth = 1;
    _copyRegion.srcSubresource = _imageSubresourceLayers;
    _copyRegion.dstSubresource = _imageSubresourceLayers;

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(currentExtent.width);
    io.DisplaySize.y = static_cast<float>(currentExtent.height);
}

void Mango::ImGuiEditor_ImplGLFWVulkan::CheckImGuiVulkanResultFn(VkResult result)
{
    M_ASSERT(result == VK_SUCCESS && "Vulkan error inside ImGui");
}

VkImageMemoryBarrier Mango::ImGuiEditor_ImplGLFWVulkan::CreateImageMemoryBarrier(
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask
)
{
    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.image = _imGuiEditorViewport->GetViewportImage();
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = 1;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    return memoryBarrier;
}
