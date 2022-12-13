#pragma once

#include "../IO/FileReader.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"

#include <vulkan/vulkan.h>

namespace Mango
{
    class GraphicsPipeline
    {
    public:
//        explicit GraphicsPipeline(LogicalDevice& logicalDevice);
        GraphicsPipeline() = default;
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline operator=(const GraphicsPipeline&) = delete;
        ~GraphicsPipeline();
        
        void CreateGraphicsPipeline(LogicalDevice& logicalDevice, Mango::SwapChain& swapChain, Mango::RenderPass& renderPass);
        VkPipeline& GetGraphicsPipeline() { return _graphicsPipeline; }
    private:
        VkShaderModule CreateShaderModule(const std::vector<char>& shaderCode);
    private:
        LogicalDevice* _logicalDevice;
        SwapChain* _swapChain;
        VkPipelineLayout _pipelineLayout {};
        VkPipeline _graphicsPipeline;
    };
}
