#include "LogicalDevice.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <set>
#include <vector>
#include <string>

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

Mango::LogicalDevice::LogicalDevice(Mango::PhysicalDevice& physicalDevice, Mango::QueueFamilyIndices& queueFamilyIndices)
{
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

    const auto createLogicalDeviceResult = vkCreateDevice(physicalDevice.GetDevice(), &createInfo, nullptr, &_logicalDevice);
    M_TRACE("Create logical device result is: " + std::to_string(createLogicalDeviceResult));
    M_ASSERT(createLogicalDeviceResult == VK_SUCCESS && "Failed to create logical device");
    
    vkGetDeviceQueue(_logicalDevice, queueFamilyIndices.GraphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_logicalDevice, queueFamilyIndices.PresentationFamily.value(), 0, &_presentationQueue);
}

Mango::LogicalDevice::~LogicalDevice()
{
    vkDestroyDevice(_logicalDevice, nullptr);
}
