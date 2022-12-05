#include "PhysicalDevice.h"

#include <stdexcept>

void Mango::PhysicalDevice::PickPhysicalDevice(Instance &instance, RenderSurface& renderSurface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    auto *physicalDevices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, physicalDevices);

    for (size_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
    {
        if (IsDeviceSuitable(physicalDevices[deviceIndex], renderSurface.GetRenderSurface()))
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

QueueFamilyIndices Mango::PhysicalDevice::FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface)
{
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    auto *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (size_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
    {
        // Check for graphics support
        if (queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = queueFamilyIndex;
        }

        // Check for presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, renderSurface, &presentSupport);
        if (presentSupport) 
        {
            indices.PresentationFamily = queueFamilyIndex;
        }
        

        if (indices.GraphicsFamily.has_value() && indices.PresentationFamily.has_value())
        {
            break;
        }
    }

    delete[] queueFamilies;
    return indices;
}

bool Mango::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR& renderSurface)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    auto queueFamilyIndices = FindQueueFamilies(device, renderSurface);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader && queueFamilyIndices.GraphicsFamily.has_value()
           && queueFamilyIndices.PresentationFamily.has_value();
}
