#include "PhysicalDevice.h"

#include <stdexcept>

void Mango::PhysicalDevice::PickPhysicalDevice(VkInstance *instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    auto *physicalDevices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(*instance, &deviceCount, physicalDevices);

    for (size_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
    {
        if (IsDeviceSuitable(physicalDevices[deviceIndex]))
        {
            _device = physicalDevices[deviceIndex];
            break;
        }
    }

    delete[] physicalDevices;
    if (_device == nullptr)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

QueueFamilyIndices Mango::PhysicalDevice::FindQueueFamilies()
{
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);
    auto *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies);

    for (size_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
    {
        if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = queueFamilyIndex;
        }
        if (indices.GraphicsFamily.has_value())
        {
            break;
        }
    }

    delete[] queueFamilies;
    return indices;
}

bool Mango::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice &device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    auto queueFamilyIndices = FindQueueFamilies();

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader && queueFamilyIndices.GraphicsFamily.has_value();
}
