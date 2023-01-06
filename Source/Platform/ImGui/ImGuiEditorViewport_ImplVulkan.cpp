#include "ImGuiEditorViewport_ImplVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::ImGuiEditorViewport_ImplVulkan::ImGuiEditorViewport_ImplVulkan(ImGuiEditorViewport_ImplVulkan_CreateInfo createInfo)
	: _physicalDevice(createInfo.PhysicalDevice), _logicalDevice(createInfo.LogicalDevice), _swapChain(createInfo.SwapChain)
{
    InitializeViewportImage();
    InitializeViewportImageView();
    InitializeImageSampler();
    _viewportImageDescriptorSet = ImGui_ImplVulkan_AddTexture(_viewportImageSampler, _viewportImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Mango::ImGuiEditorViewport_ImplVulkan::~ImGuiEditorViewport_ImplVulkan()
{
    DestroyEditorViewport();
}

void Mango::ImGuiEditorViewport_ImplVulkan::RecreateEditorViewport()
{
    DestroyEditorViewport();
    InitializeViewportImage();
    InitializeViewportImageView();
    InitializeImageSampler();
    _viewportImageDescriptorSet = ImGui_ImplVulkan_AddTexture(_viewportImageSampler, _viewportImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Mango::ImGuiEditorViewport_ImplVulkan::InitializeViewportImage()
{
    const auto& vkLogicalDevice = _logicalDevice->GetDevice();
    const auto& currentExtent = _swapChain->GetSwapChainExtent();

    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    info.extent.width = currentExtent.width;
    info.extent.height = currentExtent.height;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    auto createImageResult = vkCreateImage(vkLogicalDevice, &info, nullptr, &_viewportImage);
    M_TRACE("Create viewport image result is: " + std::to_string(createImageResult));
    M_ASSERT(createImageResult == VK_SUCCESS && "Failed to create viewport image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkLogicalDevice, _viewportImage, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = _physicalDevice->FindSuitableMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    auto allocateImageMemoryResult = vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &_viewportImageMemory);
    M_TRACE("Allocate viewport image memory result is: " + std::to_string(allocateImageMemoryResult));
    M_ASSERT(allocateImageMemoryResult == VK_SUCCESS && "Failed to allocate viewport image memory");

    auto bindImageMemoryResult = vkBindImageMemory(vkLogicalDevice, _viewportImage, _viewportImageMemory, 0);
    M_TRACE("Bind viewport image memory result is: " + std::to_string(bindImageMemoryResult));
    M_ASSERT(bindImageMemoryResult == VK_SUCCESS && "Failed to bind viewport image memory");
}

void Mango::ImGuiEditorViewport_ImplVulkan::InitializeViewportImageView()
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = _viewportImage;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    auto createImageViewResult = vkCreateImageView(_logicalDevice->GetDevice(), &imageViewCreateInfo, nullptr, &_viewportImageView);
    M_TRACE("Create viewport image view result is: " + std::to_string(createImageViewResult));
    M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create viewport image view result");
}

void Mango::ImGuiEditorViewport_ImplVulkan::InitializeImageSampler()
{
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.minLod = -1000;
    samplerCreateInfo.maxLod = 1000;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    auto createImageSamplerResult = vkCreateSampler(_logicalDevice->GetDevice(), &samplerCreateInfo, nullptr, &_viewportImageSampler);
    M_TRACE("Create viewport image sampler result is: " + std::to_string(createImageSamplerResult));
    M_ASSERT(createImageSamplerResult == VK_SUCCESS && "Failed to create viewport image sampler");
}

void Mango::ImGuiEditorViewport_ImplVulkan::DestroyEditorViewport()
{
    ImGui_ImplVulkan_RemoveTexture(_viewportImageDescriptorSet);
    const auto& vkLogicalDevice = _logicalDevice->GetDevice();
    vkDestroySampler(vkLogicalDevice, _viewportImageSampler, nullptr);
    vkDestroyImageView(vkLogicalDevice, _viewportImageView, nullptr);
    vkFreeMemory(vkLogicalDevice, _viewportImageMemory, nullptr);
    vkDestroyImage(vkLogicalDevice, _viewportImage, nullptr);
}
