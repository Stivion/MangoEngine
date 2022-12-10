#include "GraphicsPipeline.h"

#include <stdexcept>

void Mango::GraphicsPipeline::CreateGraphicsPipeline(Mango::LogicalDevice& logicalDevice)
{
    _logicalDevice = &logicalDevice;
    
    const auto vertShader = FileReader::ReadFileBytes("vert.spv");
    const auto fragShader = FileReader::ReadFileBytes("frag.spv");

    const auto vertShaderModule = CreateShaderModule(vertShader);
    const auto fragShaderModule = CreateShaderModule(fragShader);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vkDestroyShaderModule(_logicalDevice->GetDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(_logicalDevice->GetDevice(), vertShaderModule, nullptr);
}

VkShaderModule Mango::GraphicsPipeline::CreateShaderModule(const std::vector<char>& shaderCode)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_logicalDevice->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    return shaderModule;
}
