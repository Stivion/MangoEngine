#pragma once

#include "../Infrastructure/IO/FileReader.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "DescriptorSetLayout.h"

#include <vulkan/vulkan.h>
#include <string>

namespace Mango
{
    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(
            Mango::LogicalDevice& logicalDevice,
            Mango::SwapChain& swapChain,
            Mango::RenderPass& renderPass,
            Mango::DescriptorSetLayout& descriptorSetLayout,
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath
        );
        GraphicsPipeline() = delete;
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline operator=(const GraphicsPipeline&) = delete;
        ~GraphicsPipeline();
        
        const VkPipeline& GetGraphicsPipeline() const { return _graphicsPipeline; }
        const VkPipelineLayout& GetPipelineLayout() const { return _pipelineLayout; }
    private:
        static VkShaderModule CreateShaderModule(VkDevice& logicalDevice, const std::vector<char>& shaderCode);

    private:
        VkPipeline _graphicsPipeline;
        VkPipelineLayout _pipelineLayout;
        VkDevice& _logicalDevice;
    };
}
