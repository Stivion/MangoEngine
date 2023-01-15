#include "Image.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Image::Image(ImageCreateInfo createInfo) : _vulkanContext(createInfo.VulkanContext)
{
    CreateImage(createInfo.Width, createInfo.Height, createInfo.ImageFormat, createInfo.UsageFlags);
}

Mango::Image::~Image()
{
    DestroyImage();
}

void Mango::Image::RecreateImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageUsageFlags usageFlags)
{
    DestroyImage();
    CreateImage(width, height, imageFormat, usageFlags);
}

void Mango::Image::CreateImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageUsageFlags usageFlags)
{
    const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
    _imageExtent.width = width;
    _imageExtent.height = height;

    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = imageFormat;
    info.extent.width = _imageExtent.width;
    info.extent.height = _imageExtent.height;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    auto createImageResult = vkCreateImage(vkLogicalDevice, &info, nullptr, &_image);
    M_TRACE("Create viewport image result is: " + std::to_string(createImageResult));
    M_ASSERT(createImageResult == VK_SUCCESS && "Failed to create viewport image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkLogicalDevice, _image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = _vulkanContext->GetPhysicalDevice()->FindSuitableMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    auto allocateImageMemoryResult = vkAllocateMemory(vkLogicalDevice, &memoryAllocateInfo, nullptr, &_imageMemory);
    M_TRACE("Allocate viewport image memory result is: " + std::to_string(allocateImageMemoryResult));
    M_ASSERT(allocateImageMemoryResult == VK_SUCCESS && "Failed to allocate viewport image memory");

    auto bindImageMemoryResult = vkBindImageMemory(vkLogicalDevice, _image, _imageMemory, 0);
    M_TRACE("Bind viewport image memory result is: " + std::to_string(bindImageMemoryResult));
    M_ASSERT(bindImageMemoryResult == VK_SUCCESS && "Failed to bind viewport image memory");

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = _image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = imageFormat;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    auto createImageViewResult = vkCreateImageView(_vulkanContext->GetLogicalDevice()->GetDevice(), &imageViewCreateInfo, nullptr, &_imageView);
    M_TRACE("Create viewport image view result is: " + std::to_string(createImageViewResult));
    M_ASSERT(createImageViewResult == VK_SUCCESS && "Failed to create viewport image view result");
}

void Mango::Image::DestroyImage()
{
    const auto& vkLogicalDevice = _vulkanContext->GetLogicalDevice()->GetDevice();
    vkDestroyImageView(vkLogicalDevice, _imageView, nullptr);
    vkFreeMemory(vkLogicalDevice, _imageMemory, nullptr);
    vkDestroyImage(vkLogicalDevice, _image, nullptr);
}
