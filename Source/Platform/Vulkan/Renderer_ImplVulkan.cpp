#include "Renderer_ImplVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Renderer_ImplVulkan::Renderer_ImplVulkan(const Renderer_ImplVulkan_CreateInfo createInfo)
	: Renderer()
{
    _maxFramesInFlight = createInfo.MaxFramesInFlight;
    _vulkanContext = createInfo.VulkanContext;
    _renderArea = createInfo.RenderArea;
    _renderAreaInfo = createInfo.RenderAreaInfo;

    Mango::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.ImageFormat = _renderAreaInfo.ImageFormat;
    renderPassCreateInfo.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    renderPassCreateInfo.ColorAttachmentReferenceLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    _renderPass = std::make_unique<Mango::RenderPass>(*_vulkanContext->GetLogicalDevice(), renderPassCreateInfo);

    _descriptorPool = std::make_unique<Mango::DescriptorPool>(_poolSizes, *_vulkanContext->GetLogicalDevice());

    // All descriptor set layouts is owned by renderer
    {
        Mango::DescriptorSetLayoutBuilder builder(*_vulkanContext->GetLogicalDevice());
        _globalDescriptorSetLayout = builder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .Build();
    }
    {
        Mango::DescriptorSetLayoutBuilder builder(*_vulkanContext->GetLogicalDevice());
        _perModelDescriptorSetLayout = builder
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT)
            .Build();
    }
    std::vector<const Mango::DescriptorSetLayout*> layouts{ _globalDescriptorSetLayout.get(), _perModelDescriptorSetLayout.get() };
    _descriptorPool->AllocateDescriptorSets(layouts);
    _graphicsPipeline = std::make_unique<Mango::GraphicsPipeline>(*_vulkanContext->GetLogicalDevice(), *_renderPass, layouts, "vert.spv", "frag.spv");

    _commandPool = std::make_unique<Mango::CommandPool>(*_vulkanContext->GetLogicalDevice(), *_vulkanContext->GetQueueFamilyIndices());
    _commandBuffers = std::make_unique<Mango::CommandBuffersPool>(
        _maxFramesInFlight,
        *_vulkanContext->GetLogicalDevice(),
        *_renderPass,
        *_commandPool,
        _renderArea
    );
    _framebuffers = std::make_unique<Mango::FramebuffersPool>(*_vulkanContext->GetLogicalDevice(), *_renderPass, _renderArea, _renderAreaInfo);

    _uniformBuffers = std::make_unique<Mango::UniformBuffersPool>(*_vulkanContext->GetPhysicalDevice(), *_vulkanContext->GetLogicalDevice());
    for (uint32_t i = 0; i < _maxFramesInFlight; i++)
    {
        Mango::UniformBufferCreateInfo createInfo{};
        createInfo.BufferId = i;
        createInfo.Binding = 0;
        createInfo.BufferSize = sizeof(UniformBufferObject);
        createInfo.DestinationSet = _descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId());
        createInfo.DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        _uniformBuffers->CreateBuffer(createInfo);

        // NOTE: This could be reallocated during rendering which could slow performance
        Mango::UniformBufferCreateInfo dynamicBufferCreateInfo{};
        dynamicBufferCreateInfo.BufferId = static_cast<uint64_t>(i) + _maxFramesInFlight;
        dynamicBufferCreateInfo.Binding = 0;
        dynamicBufferCreateInfo.BufferSize = sizeof(DynamicUniformBufferObject);
        dynamicBufferCreateInfo.DestinationSet = _descriptorPool->GetDescriptorSet(_perModelDescriptorSetLayout->GetId());
        dynamicBufferCreateInfo.DescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        _uniformBuffers->CreateBuffer(dynamicBufferCreateInfo);
    }

    _descriptorSets.clear();
    _descriptorSets.push_back(_descriptorPool->GetDescriptorSet(_globalDescriptorSetLayout->GetId()));
    _descriptorSets.push_back(_descriptorPool->GetDescriptorSet(_perModelDescriptorSetLayout->GetId()));
}

void Mango::Renderer_ImplVulkan::BeginFrame(uint32_t currentFrame)
{
    _currentFrame = currentFrame;
}

void Mango::Renderer_ImplVulkan::EndFrame()
{
    _renderData.Reset();
}

void Mango::Renderer_ImplVulkan::HandleResize(Mango::RenderArea renderArea, Mango::RenderAreaInfo renderAreaInfo)
{
    // On resize we must update _renderArea and _renderAreaInfo and recreate render pass and framebuffers
    _renderArea = renderArea;
    _renderAreaInfo = renderAreaInfo;

    VkExtent2D extent{};
    extent.width = _renderArea.Width;
    extent.height = _renderArea.Height;
    _renderPass->RecreateRenderPass(*_vulkanContext->GetLogicalDevice(), _renderAreaInfo.ImageFormat);
    const auto& imageViews = _renderAreaInfo.ImageViews;
    M_ASSERT(imageViews.size() == _framebuffers->GetFramebuffersCount() && "Framebuffers count and image views count doesn't match");
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        _framebuffers->GetFramebuffer(i).RecreateFramebuffer(*_renderPass, extent, imageViews[i]);
    }
}

const Mango::CommandBuffer& Mango::Renderer_ImplVulkan::RecordCommandBuffer(uint32_t imageIndex)
{
    const auto& currentCommandBuffer = _commandBuffers->GetCommandBuffer(_currentFrame);
    const auto& currentFramebuffer = _framebuffers->GetFramebuffer(imageIndex);

    currentCommandBuffer.Reset();
    currentCommandBuffer.BeginCommandBuffer();

    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    memoryBarrier.image = _renderAreaInfo.Images[0];
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = 1;
    memoryBarrier.srcAccessMask = VK_ACCESS_NONE;
    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(
        currentCommandBuffer.GetVkCommandBuffer(),
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &memoryBarrier
    );

    currentCommandBuffer.BeginRenderPass(
        currentFramebuffer.GetSwapChainFramebuffer(),
        _renderArea
    );
    currentCommandBuffer.BindPipeline(*_graphicsPipeline);

    // Group all verices into buffers
    const uint32_t vertexCount = static_cast<uint32_t>(_renderData.Vertices.size());
    const uint32_t indicesCount = static_cast<uint32_t>(_renderData.Indices.size());
    
    if (vertexCount == 0)
    {
        currentCommandBuffer.EndRenderPass();
        currentCommandBuffer.EndCommandBuffer();
        return currentCommandBuffer;
    }

    _vertexBuffer = std::make_unique<Mango::VertexBuffer>(vertexCount, sizeof(Vertex) * vertexCount, _renderData.Vertices.data(), *_vulkanContext->GetPhysicalDevice(), *_vulkanContext->GetLogicalDevice(), *_commandPool);
    _indexBuffer = std::make_unique<Mango::IndexBuffer>(indicesCount, sizeof(uint16_t) * indicesCount, _renderData.Indices.data(), *_vulkanContext->GetPhysicalDevice(), *_vulkanContext->GetLogicalDevice(), *_commandPool);
    
    UpdateGlobalDescriptorSets();
    UpdatePerModelDescriptorSets();

    // Draw call
    currentCommandBuffer.DrawIndexed(
        *_vertexBuffer,
        *_indexBuffer,
        _renderData.IndicesPerDraw,
        _renderData.DynamicOffsets,
        _descriptorSets,
        _graphicsPipeline->GetPipelineLayout()
    );

    // End frame
    currentCommandBuffer.EndRenderPass();
    currentCommandBuffer.EndCommandBuffer();
    return currentCommandBuffer;
}

void Mango::Renderer_ImplVulkan::DrawRect(glm::mat4 transform, glm::vec4 color)
{
    const uint32_t vertexCount = static_cast<uint32_t>(_renderData.RectangleVertices.size());
    
    const uint32_t lastVertexCount = _renderData.Vertices.size();
    for (size_t i = 0; i < vertexCount; i++)
    {
        Mango::Vertex vertex{ _renderData.RectangleVertices[i], color };
        _renderData.Vertices.push_back(vertex);
    }
    for (size_t i = 0; i < _renderData.RectangleIndices.size(); i++)
    {
        _renderData.Indices.push_back(lastVertexCount + _renderData.RectangleIndices[i]);
    }
    _renderData.IndicesPerDraw.push_back(static_cast<uint32_t>(_renderData.RectangleIndices.size()));
    _renderData.Transforms.push_back(transform);
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
    _renderData.IndicesPerDraw.push_back(static_cast<uint32_t>(_renderData.TriangleVertices.size()));
    _renderData.Transforms.push_back(transform);
}

void Mango::Renderer_ImplVulkan::SetCamera(RendererCameraInfo cameraInfo)
{
    _cameraInfo = cameraInfo;
}

void Mango::Renderer_ImplVulkan::UpdateGlobalDescriptorSets()
{
    Mango::UniformBufferObject ubo{};
    ubo.View = glm::lookAt(_cameraInfo.Position, _cameraInfo.ViewTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.Projection = glm::perspective(glm::radians(_cameraInfo.FovDegrees), _renderArea.Width / static_cast<float>(_renderArea.Height), _cameraInfo.NearPlane, _cameraInfo.FarPlane);
    ubo.Projection[1][1] *= -1;

    const auto& uniformBuffer = _uniformBuffers->GetUniformBuffer(_currentFrame);
    void* gpuMemory = uniformBuffer.MapMemory();
    memcpy(gpuMemory, &ubo, sizeof(ubo));
    uniformBuffer.UnmapMemory();
}

void Mango::Renderer_ImplVulkan::UpdatePerModelDescriptorSets()
{
    const uint32_t objectsCount = static_cast<uint64_t>(_renderData.Transforms.size());
    Mango::UniformBuffer& buffer = _uniformBuffers->GetUniformBuffer(static_cast<uint64_t>(_currentFrame) + _maxFramesInFlight);
    const VkDeviceSize objectSize = static_cast<VkDeviceSize>(sizeof(glm::mat4));
    const auto requiredCapacity = buffer.GetAlignedSize(objectsCount * objectSize);
    const bool bufferResized = buffer.EnsureCapacity(requiredCapacity);
    if (bufferResized)
    {
        const auto descriptorSet = _descriptorPool->GetDescriptorSet(_perModelDescriptorSetLayout->GetId());
        _uniformBuffers->UpdateDescriptorSet(buffer, descriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    }

    char* dynamicGpuMemory = static_cast<char*>(buffer.MapMemory());
    const auto memoryOffset = buffer.GetAlignedSize(objectSize);
    for (uint32_t i = 0; i < objectsCount; i++)
    {
        Mango::DynamicUniformBufferObject dUbo{};
        dUbo.Model = _renderData.Transforms[i];
        memcpy(dynamicGpuMemory, &dUbo, sizeof(dUbo));
        dynamicGpuMemory += memoryOffset;
        _renderData.DynamicOffsets.push_back(memoryOffset * i);
    }
    buffer.UnmapMemory();
}
