#pragma once

#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace Mango
{
	struct RenderArea
	{
		// Left upper offset
		int32_t X, Y;
		// Width and height of a render area
		uint32_t Width, Height;
	};

	struct RenderAreaInfo
	{
		VkFormat ImageFormat;
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		std::vector<VkImageView> ImageViews;
		std::vector<VkImage> Images;
	};
}
