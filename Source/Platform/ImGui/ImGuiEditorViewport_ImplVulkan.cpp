#include "ImGuiEditorViewport_ImplVulkan.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::ImGuiEditorViewport_ImplVulkan::ImGuiEditorViewport_ImplVulkan(ImGuiEditorViewport_ImplVulkan_CreateInfo createInfo)
    : _vulkanContext(createInfo.VulkanContext), _renderArea(createInfo.RenderArea), _renderAreaInfo(createInfo.RenderAreaInfo)
{
    Mango::ImageCreateInfo imageCreateInfo{};
    imageCreateInfo.Width = _renderArea.Width;
    imageCreateInfo.Height = _renderArea.Height;
    imageCreateInfo.VulkanContext = _vulkanContext;
    imageCreateInfo.ImageFormat = _renderAreaInfo.ImageFormat;
    imageCreateInfo.UsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    _image = std::make_unique<Mango::Image>(imageCreateInfo);
    InitializeImageSampler();
    _viewportImageDescriptorSet = ImGui_ImplVulkan_AddTexture(_viewportImageSampler, _image->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Mango::ImGuiEditorViewport_ImplVulkan::~ImGuiEditorViewport_ImplVulkan()
{
    DestroyEditorViewport();
}

void Mango::ImGuiEditorViewport_ImplVulkan::RecreateEditorViewport(const Mango::RenderArea renderArea, const Mango::RenderAreaInfo renderAreaInfo)
{
    DestroyEditorViewport();
    _renderArea = renderArea;
    _renderAreaInfo = renderAreaInfo;
    Mango::ImageCreateInfo imageCreateInfo{};
    imageCreateInfo.Width = _renderArea.Width;
    imageCreateInfo.Height = _renderArea.Height;
    imageCreateInfo.VulkanContext = _vulkanContext;
    imageCreateInfo.ImageFormat = _renderAreaInfo.ImageFormat;
    imageCreateInfo.UsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    _image = std::make_unique<Mango::Image>(imageCreateInfo);
    InitializeImageSampler();
    _viewportImageDescriptorSet = ImGui_ImplVulkan_AddTexture(_viewportImageSampler, _image->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
    auto createImageSamplerResult = vkCreateSampler(_vulkanContext->GetLogicalDevice()->GetDevice(), &samplerCreateInfo, nullptr, &_viewportImageSampler);
    M_TRACE("Create viewport image sampler result is: " + std::to_string(createImageSamplerResult));
    M_ASSERT(createImageSamplerResult == VK_SUCCESS && "Failed to create viewport image sampler");
}

void Mango::ImGuiEditorViewport_ImplVulkan::DestroyEditorViewport()
{
    ImGui_ImplVulkan_RemoveTexture(_viewportImageDescriptorSet);
    const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
    vkDestroySampler(vkLogicalDevice, _viewportImageSampler, nullptr);
}
