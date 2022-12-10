#include "PhysicalDevice.h"

#include "SwapChainSupportDetails.h"

#include <stdexcept>

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

void Mango::PhysicalDevice::PickPhysicalDevice(Instance &instance, RenderSurface &renderSurface)
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

QueueFamilyIndices Mango::PhysicalDevice::FindQueueFamilies(VkPhysicalDevice &device, VkSurfaceKHR &renderSurface)
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

bool Mango::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice &device, VkSurfaceKHR &renderSurface)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    auto queueFamilyIndices = FindQueueFamilies(device, renderSurface);
    auto extensionsAvailable = CheckDeviceExtensions(device, requiredDeviceExtensions);
    auto swapChainDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(device, renderSurface);
    auto isSwapChainSuitable = false;
    if (extensionsAvailable) 
    {
        isSwapChainSuitable = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
    }

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
    && deviceFeatures.geometryShader
    && queueFamilyIndices.GraphicsFamily.has_value()
    && queueFamilyIndices.PresentationFamily.has_value()
    && extensionsAvailable
    && isSwapChainSuitable;
}

bool Mango::PhysicalDevice::CheckDeviceExtensions(VkPhysicalDevice& device, const std::vector<const char*> &extensions)
{
    uint32_t extensionsCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

    for (const auto& extension : extensions)
    {
        bool extensionFound = false;
        for (size_t extensionIndex = 0; extensionIndex < extensionsCount; extensionIndex++)
        {
            if (strcmp(extension, availableExtensions[extensionIndex].extensionName) == 0)
            {
                extensionFound = true;
                break;
            }
        }
        
        if (!extensionFound)
        {
            return false;
        }
    }
    
    return true;
}
