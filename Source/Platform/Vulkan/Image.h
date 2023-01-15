#pragma once

#include "Context.h"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Mango
{
	struct ImageCreateInfo
	{
		const Mango::Context* VulkanContext;
		uint32_t Width;
		uint32_t Height;
		VkFormat ImageFormat;
		VkImageUsageFlags UsageFlags;
	};

	class Image
	{
	public:
		Image(ImageCreateInfo createInfo);
		Image(const Image&) = delete;
		Image operator=(const Image&) = delete;
		~Image();

		void RecreateImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageUsageFlags usageFlags);

		inline const VkImage& Get() const { return _image; }
		inline const VkImageView& GetImageView() const { return _imageView; }
		inline const uint32_t GetWidth() const { return _imageExtent.width; }
		inline const uint32_t GetHeight() const { return _imageExtent.height; }

	private:
		const Mango::Context* _vulkanContext;

	private:
		VkImage _image;
		VkDeviceMemory _imageMemory;
		VkImageView _imageView;
		VkExtent2D _imageExtent;

	private:
		void CreateImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageUsageFlags usageFlags);
		void DestroyImage();
	};
}
