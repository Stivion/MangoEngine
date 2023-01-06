#pragma once

#include <vulkan/vulkan.hpp>

namespace Mango
{
    class Instance
    {
    public:
        Instance();
        Instance(const Instance&) = delete;
        Instance operator=(const Instance&) = delete;
        ~Instance();

        const VkInstance& GetInstance() const { return _instance; }
    private:
        VkInstance _instance;
        
    private:
        static bool CheckRequiredExtensions(const char** extensions, const uint32_t requiredExtensionsCount);
        static bool CheckValidationLayerSupport();
    };
}

