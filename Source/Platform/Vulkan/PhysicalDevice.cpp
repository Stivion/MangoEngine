#include "PhysicalDevice.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

Mango::PhysicalDevice::PhysicalDevice(Mango::Instance& instance, Mango::RenderSurface& renderSurface)
{
    auto vkInstance = instance.GetInstance();
    auto vkRenderSurface = renderSurface.GetRenderSurface();

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    M_TRACE("Found " + std::to_string(deviceCount) + " GPUs with Vulkan support");
    M_ASSERT(deviceCount != 0 && "Failed to find GPUs with Vulkan support");

    auto* physicalDevices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, physicalDevices);

    _physicalDevice = VK_NULL_HANDLE;
    for (size_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
    {
        if (IsDeviceSuitable(physicalDevices[deviceIndex], vkRenderSurface))
        {
            _physicalDevice = physicalDevices[deviceIndex];
            break;
        }
    }

    delete[] physicalDevices;
    M_ASSERT(_physicalDevice != VK_NULL_HANDLE && "Failed to find a suitable GPU with Vulkan support");
}

uint32_t Mango::PhysicalDevice::FindSuitableMemoryType(uint32_t memoryType, VkMemoryPropertyFlags requiredProperties) const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((memoryType & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
        {
            return i;
        }
    }

    M_ASSERT(false && "Unable to find suitable memory type");
}

bool Mango::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice &device, VkSurfaceKHR &renderSurface)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    auto queueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(device, renderSurface);
    auto extensionsAvailable = CheckDeviceExtensions(device, requiredDeviceExtensions);
    auto swapChainDetails = Mango::SwapChainSupportDetails::QuerySwapChainSupport(device, renderSurface);
    auto isSwapChainSuitable = false;
    if (extensionsAvailable) 
    {
        isSwapChainSuitable = !swapChainDetails.Formats.empty() && !swapChainDetails.PresentModes.empty();
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
