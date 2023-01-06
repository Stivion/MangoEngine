#pragma once

#include "../../Infrastructure/IO/FileReader.h"
#include "LogicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "DescriptorSetLayout.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Mango
{
    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(
            const Mango::LogicalDevice& logicalDevice,
            const Mango::RenderPass& renderPass,
            std::vector<const Mango::DescriptorSetLayout*>& descriptorSetLayouts,
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
        static VkShaderModule CreateShaderModule(const VkDevice& logicalDevice, const std::vector<char>& shaderCode);

    private:
        VkPipeline _graphicsPipeline;
        VkPipelineLayout _pipelineLayout;
        const VkDevice& _logicalDevice;
    };
}
