#pragma once

#include "LogicalDevice.h"
#include "SwapChain.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Mango
{
    class ImageView
    {
    public:
        ImageView() = default;
        ImageView(const ImageView&) = delete;
        ImageView operator=(const ImageView&) = delete;
        ~ImageView();
        
        void CreateImageViews(Mango::LogicalDevice& logicalDevice, Mango::SwapChain& swapChain);
    private:
        std::vector<VkImageView> _swapChainImageViews;
        VkDevice* _logicalDevice;
    };
}
