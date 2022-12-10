#include "LogicalDevice.h"

#include <set>
#include <vector>
#include <stdexcept>

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

Mango::LogicalDevice::~LogicalDevice()
{
    vkDestroyDevice(_device, nullptr);
}

void Mango::LogicalDevice::CreateLogicalDevice(PhysicalDevice& physicalDevice, RenderSurface& renderSurface)
{
    auto queueFamilyIndices = Mango::PhysicalDevice::FindQueueFamilies(physicalDevice.GetDevice(), renderSurface.GetRenderSurface());
    std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.GraphicsFamily.value(), queueFamilyIndices.PresentationFamily.value() };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    
    float queuePriority = 1.0f;
    for (const auto& queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    if (vkCreateDevice(physicalDevice.GetDevice(), &createInfo, nullptr, &_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical _device");
    }

    vkGetDeviceQueue(_device, queueFamilyIndices.GraphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, queueFamilyIndices.PresentationFamily.value(), 0, &_presentationQueue);
}
