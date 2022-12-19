#include "Instance.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

Mango::Instance::Instance()
{
    const auto validationLayersAvailable = enableValidationLayers && CheckValidationLayerSupport();
    M_ASSERT(validationLayersAvailable && "Validation layers enabled, but not available");

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Mango Engine";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Mango";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfwExtensionsCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    M_TRACE("GLFW required extensions count: " + std::to_string(glfwExtensionsCount));
    M_ASSERT(CheckRequiredExtensions(glfwExtensions, glfwExtensionsCount) && "One or more required extension layers is missing");

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = glfwExtensionsCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    if (enableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }

    const auto createInstanceResult = vkCreateInstance(&instanceCreateInfo, nullptr, &_instance);
    M_TRACE("Vulkan instance create result is: " + std::to_string(createInstanceResult));
    M_ASSERT(createInstanceResult == VK_SUCCESS && "Failed to create Vulkan Instance");
}

Mango::Instance::~Instance()
{
    vkDestroyInstance(_instance, nullptr);
}

bool Mango::Instance::CheckRequiredExtensions(const char **requiredExtensions, const uint32_t requiredExtensionsCount)
{
    uint32_t extensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
    auto *extensions = new VkExtensionProperties[extensionsCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions);

    for (size_t requiredExtensionIndex = 0; requiredExtensionIndex < requiredExtensionsCount; requiredExtensionIndex++)
    {
        bool layerFound = false;
        auto &requiredExtension = requiredExtensions[requiredExtensionIndex];

        for (size_t extensionIndex = 0; extensionIndex < extensionsCount; extensionIndex++)
        {
            auto &existingExtension = extensions[extensionIndex];
            if (strcmp(requiredExtension, existingExtension.extensionName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    delete[] extensions;
    return true;
}

bool Mango::Instance::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    auto *availableLayers = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (const char *layerName: validationLayers)
    {
        bool layerFound = false;
        for (size_t layerIndex = 0; layerIndex < layerCount; layerIndex++)
        {
            if (strcmp(layerName, availableLayers[layerIndex].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    delete[] availableLayers;
    return true;
}
