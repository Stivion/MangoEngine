#pragma once

#include <vulkan/vulkan.hpp>

namespace Mango
{
    class Instance
    {
    public:
        Instance() = default;
        Instance(const Instance&) = delete;
        Instance operator=(const Instance&) = delete;
        ~Instance();
        
        void CreateInstance();
        
        VkInstance& GetInstance() { return _instance; }
    
    private:
        VkInstance _instance;
    };
}

