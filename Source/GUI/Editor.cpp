#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>

void Mango::Editor::InitializeImGui()
{
    //auto& vkPhysicalDevice = _physicalDevice.GetDevice();
    //auto& vkLogicalDevice = _logicalDevice.GetDevice();
    //auto& vkRenderSurface = _renderSurface.GetRenderSurface();

    //VkDescriptorPoolSize pool_sizes[] =
    //{
    //    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    //    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    //    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    //    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    //    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    //    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    //};
    //VkDescriptorPoolCreateInfo pool_info = {};
    //pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    //pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    //pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    //pool_info.pPoolSizes = pool_sizes;
    //std::cout << "Creating descriptor pool" << std::endl;
    //if (vkCreateDescriptorPool(vkLogicalDevice, &pool_info, nullptr, &_descriptorPool) != VK_SUCCESS)
    //{
    //    throw std::runtime_error("Failed to create descriptor pool");
    //}

    //const auto& imageFormats = _swapChain.GetSwapChainSurfaceFormat().format;
    //std::cout << "Setting ImGui window" << std::endl;

    //std::cout << "Creating ImGui context" << std::endl;
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize.x = _window.GetWindowWidth();
    //io.DisplaySize.y = _window.GetWindowHeight();
    //ImGui::StyleColorsDark();

    //std::cout << "Initializing ImGui for GLFW" << std::endl;
    //ImGui_ImplGlfw_InitForVulkan(_window.GetWindow(), true);
    //ImGui_ImplVulkan_InitInfo initInfo{};
    //initInfo.Instance = _instance.GetInstance();
    //initInfo.PhysicalDevice = vkPhysicalDevice;
    //initInfo.Device = vkLogicalDevice;
    //initInfo.QueueFamily = _queueFamilyIndices.GraphicsFamily.value();
    //initInfo.Queue = _logicalDevice.GetGraphicsQueue();
    //initInfo.PipelineCache = nullptr;
    //initInfo.DescriptorPool = _descriptorPool;
    //initInfo.Subpass = 0;
    //initInfo.MinImageCount = _swapChainSupportDetails.SurfaceCapabilities.minImageCount;
    //initInfo.ImageCount = _swapChain.GetSwapChainImages().size();
    //initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    //initInfo.Allocator = nullptr;
    //initInfo.CheckVkResultFn = nullptr;
    //std::cout << "Initializing ImGui for Vulkan" << std::endl;
    //ImGui_ImplVulkan_Init(&initInfo, _renderPass.GetRenderPass());

    //// Use any command queue
    //VkCommandPool command_pool = _commandPool.GetCommandPool();
    //VkCommandBuffer command_buffer = _commandBuffer.GetCommandBuffer();

    //if (vkResetCommandPool(vkLogicalDevice, command_pool, 0) != VK_SUCCESS)
    //{
    //    throw std::runtime_error("Failed to reset command pool (fonts)");
    //}
    //VkCommandBufferBeginInfo begin_info = {};
    //begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    //if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    //{
    //    throw std::runtime_error("Failed to begin command buffer (fonts)");
    //}

    //std::cout << "Creating fonts" << std::endl;
    //ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    //VkSubmitInfo end_info = {};
    //end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //end_info.commandBufferCount = 1;
    //end_info.pCommandBuffers = &command_buffer;
    //if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    //{
    //    throw std::runtime_error("Failed to end command buffer (fonts)");
    //}
    //if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS)
    //{
    //    throw std::runtime_error("Failed to submit graphics queue (fonts)");
    //}

    //vkDeviceWaitIdle(vkLogicalDevice);
    //ImGui_ImplVulkan_DestroyFontUploadObjects();
}
