#include "ImGuiEditor_ImplGLFWVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::ImGuiEditor_ImplGLFWVulkan::ImGuiEditor_ImplGLFWVulkan(ImGuiEditor_ImplGLFWVulkan_CreateInfo createInfo) : ImGuiEditor(createInfo.Window)
{
    const auto& instance = *createInfo.Instance;
    const auto& physicalDevice = *createInfo.PhysicalDevice;
    const auto& renderSurface = *createInfo.RenderSurface;
    const auto& logicalDevice = *createInfo.LogicalDevice;
    const auto& queueFamilyIndices = *createInfo.QueueFamilyIndices;
    const auto& swapChain = *createInfo.SwapChain;
    const auto& graphicsPipeline = *createInfo.GraphicsPipeline;
    Mango::SwapChainSupportDetails swapChainSupportDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(
        physicalDevice.GetDevice(),
        renderSurface.GetRenderSurface()
    );

    Mango::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    _imGuiRenderPass = std::make_unique<Mango::RenderPass>(logicalDevice, swapChain, renderPassCreateInfo);
    _imGuiFramebuffers = std::make_unique<Mango::FramebuffersPool>(logicalDevice, *_imGuiRenderPass, swapChain);
    _imGuiDescriptorPool = std::make_unique<Mango::DescriptorPool>(_imGuiPoolSizes, logicalDevice);
    _imGuiCommandPool = std::make_unique<Mango::CommandPool>(logicalDevice, queueFamilyIndices);
    _imGuiCommandBuffers = std::make_unique<Mango::CommandBuffersPool>(
        createInfo.MaxFramesInFlight,
        logicalDevice,
        swapChain,
        *_imGuiRenderPass,
        *_imGuiCommandPool,
        graphicsPipeline
    );

    const auto& vkInstance = instance.GetInstance();
    const auto& vkPhysicalDevice = physicalDevice.GetDevice();
    const auto& vkLogicalDevice = logicalDevice.GetDevice();
    const auto& vkGraphicsQueueFamily = queueFamilyIndices.GraphicsFamily.value();
    const auto& vkGraphicsQueue = logicalDevice.GetGraphicsQueue();
    const auto& vkDescriptorPool = _imGuiDescriptorPool->GetDescriptorPool();
    const auto& vkMinImageCount = swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    const auto& vkImageCount = static_cast<uint32_t>(swapChain.GetSwapChainImages().size());
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

    Mango::ImGuiEditorViewport_ImplVulkan_CreateInfo editorViewportCreateInfo{};
    editorViewportCreateInfo.PhysicalDevice = createInfo.PhysicalDevice;
    editorViewportCreateInfo.LogicalDevice = createInfo.LogicalDevice;
    editorViewportCreateInfo.SwapChain = createInfo.SwapChain;
    _imGuiEditorViewport = std::make_unique<Mango::ImGuiEditorViewport_ImplVulkan>(editorViewportCreateInfo);
    _viewportTextureId = _imGuiEditorViewport->GetViewportImageDescriptorSet();
}

Mango::ImGuiEditor_ImplGLFWVulkan::~ImGuiEditor_ImplGLFWVulkan()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::NewFrame() const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    Mango::ImGuiEditor::NewFrame();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::EndFrame() const
{
    Mango::ImGuiEditor::EndFrame();
}

void Mango::ImGuiEditor_ImplGLFWVulkan::Draw() const
{
}

void Mango::ImGuiEditor_ImplGLFWVulkan::CheckImGuiVulkanResultFn(VkResult result)
{
    M_ASSERT(result == VK_SUCCESS && "Vulkan error inside ImGui");
}
