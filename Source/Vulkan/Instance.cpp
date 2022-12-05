#include "Instance.h"

#include <GLFW/glfw3.h>

#include <vector>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

bool CheckRequiredExtensions(const char **extensions, uint32_t requiredExtensionsCount);

bool CheckValidationLayerSupport();

Mango::Instance::~Instance()
{
    vkDestroyInstance(_instance, nullptr);
}

void Mango::Instance::CreateInstance()
{
    if (enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers enabled, but not available");
    }

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Vulkan learning";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfwExtensionsCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    if (!CheckRequiredExtensions(glfwExtensions, glfwExtensionsCount))
    {
        throw std::runtime_error("One or more required extension layers is missing");
    }

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = glfwExtensionsCount;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
    if (enableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    } else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Instance");
    }
}

bool CheckRequiredExtensions(const char **requiredExtensions, uint32_t requiredExtensionsCount)
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

bool CheckValidationLayerSupport()
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
