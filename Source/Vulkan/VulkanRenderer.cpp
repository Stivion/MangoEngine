#include "VulkanRenderer.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::VulkanRenderer::VulkanRenderer(bool isOffscreen, const VulkanRendererCreateInfo createInfo)
	: Renderer(), _hardwareInfo(createInfo.HardwareInfo), _renderSurface(createInfo.RenderSurface), _logicalDevice(createInfo.LogicalDevice), _swapChain(createInfo.SwapChain), _maxFramesInFlight(createInfo.MaxFramesInFlight)
{
	// Create render pass
	if (isOffscreen)
	{
		_renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		_renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	else
	{
		_renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	}
	_renderPass = std::make_unique<Mango::RenderPass>(createInfo.LogicalDevice, createInfo.SwapChain, _renderPassCreateInfo);
    _descriptorPool = std::make_unique<Mango::DescriptorPool>(_poolSizes, _logicalDevice);

    // All descriptor set layouts is owned by renderer
    Mango::DescriptorSetLayoutBuilder globalLayoutBuilder(_logicalDevice);
    std::unique_ptr<Mango::DescriptorSetLayout> globalLayout = globalLayoutBuilder
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .Build();

    Mango::DescriptorSetLayoutBuilder textureLayoutBuilder(_logicalDevice);
    std::unique_ptr<Mango::DescriptorSetLayout> textureLayout = textureLayoutBuilder
        .AddBinding(0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
        .AddBinding(1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
        .AddBinding(2, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
        .AddBinding(3, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build();

    Mango::DescriptorSetLayoutBuilder drawLayoutBuilder(_logicalDevice);
    std::unique_ptr<Mango::DescriptorSetLayout> drawLayout = drawLayoutBuilder
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .Build();

    std::vector<const Mango::DescriptorSetLayout*> layouts{ globalLayout.get(), textureLayout.get(), drawLayout.get() };
    _descriptorPool->AllocateDescriptorSets(layouts);

    _graphicsPipeline = std::make_unique<Mango::GraphicsPipeline>(_logicalDevice, *_renderPass, layouts, "vert.spv", "frag.spv");

    // Create framebuffers
    _framebuffers = std::make_unique<Mango::FramebuffersPool>(_logicalDevice, *_renderPass, _swapChain);

    // Create command buffers
    _commandPool = std::make_unique<Mango::CommandPool>(_logicalDevice, _hardwareInfo.QueueFamilyIndices);
    _commandBuffers = std::make_unique<Mango::CommandBuffersPool>(_maxFramesInFlight, _logicalDevice, _swapChain, *_renderPass, *_commandPool, *_graphicsPipeline);

	// Create fences
	_fences.resize(_maxFramesInFlight);
	for (size_t i = 0; i < _maxFramesInFlight; i++)
	{
		_fences[i] = std::make_shared<Mango::Fence>(false, _logicalDevice);
	}

	// Create semaphores
	_imageAvailableSemaphores.resize(_maxFramesInFlight);
	_renderFinishedSemaphores.resize(_maxFramesInFlight);
	for (size_t i = 0; i < _maxFramesInFlight; i++)
	{
		_imageAvailableSemaphores[i] = std::make_shared<Mango::Semaphore>(_logicalDevice);
		_renderFinishedSemaphores[i] = std::make_shared<Mango::Semaphore>(_logicalDevice);
	}
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
	const auto& vkLogicalDevice = _logicalDevice.GetDevice();

    _fences[_currentFrame]->WaitForFence();
    
    // Acquire next image to render to
    uint32_t imageIndex;
    auto nextImageResult = vkAcquireNextImageKHR(
        vkLogicalDevice,
        _swapChain.GetSwapChain(),
        UINT64_MAX,
        _imageAvailableSemaphores[_currentFrame]->GetSemaphore(),
        VK_NULL_HANDLE,
        &imageIndex
    );
    if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || nextImageResult == VK_SUBOPTIMAL_KHR)
    {
        HandleFramebuffersResized();
    }
    else
    {
        M_ASSERT(nextImageResult == VK_SUCCESS && "Failed to acquire next swap chain image");
    }

    // TODO: Handle minimized window
    _fences[_currentFrame]->ResetFence();

    auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);
    currentCommandBuffer.Reset();

    // Start command buffer recording

    // Draw

    // End command buffer recording

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
