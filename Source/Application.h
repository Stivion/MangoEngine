#pragma once

#include "Windowing/Window.h"
#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/RenderSurface.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Framebuffer.h"
#include "Vulkan/FramebuffersPool.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/VertexBuffer.h"
#include "Vulkan/IndexBuffer.h"
#include "Vulkan/CommandBuffersPool.h"
#include "Vulkan/Vertex.h"
#include "Vulkan/UniformBuffer.h"
#include "Vulkan/UniformBuffersPool.h"
#include "Vulkan/UniformBufferObject.h"
#include "Vulkan/DescriptorPool.h"
#include "Vulkan/DescriptorSetLayout.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/HardwareInfo.h"

#include "Render/Renderer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <cstdint>
#include <vector>
#include <array>
#include <memory>

namespace Mango
{
    class Application
    {
    public:
        Application();
        Application(const Application&) = delete;
        Application operator=(const Application&) = delete;
        ~Application();
        
        void Run();

    private:
        const static uint32_t MaxFramesInFlight = 1;

        void InitializeWindow();
        void InitializeVulkan();
        void InitializeImGui();
        void RunMainLoop();
        void DrawFrame(uint32_t currentFrame);

        static void FramebufferResizedCallback(GLFWwindow* window, int width, int height);
        static void CheckImGuiVulkanResultFn(VkResult result);
        
    private:
        // Windowing
        Mango::Window _window{ 800, 600 };
        bool _framebufferResized = false;

        // Vulkan
        const std::vector<VkDescriptorPoolSize> _poolSizes =
        {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 4 }
        };

        Mango::Instance _instance{};
        Mango::RenderSurface _renderSurface{ _window, _instance };
        Mango::PhysicalDevice _physicalDevice{ _instance, _renderSurface };
        Mango::QueueFamilyIndices _queueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        Mango::LogicalDevice _logicalDevice{ _physicalDevice, _queueFamilyIndices };
        Mango::SwapChainSupportDetails _swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        Mango::SwapChain _swapChain{ _window, _renderSurface, _logicalDevice, _swapChainSupportDetails, _queueFamilyIndices };
        
        Mango::HardwareInfo _hardwareInfo{ _instance, _physicalDevice, _queueFamilyIndices };
        Mango::VulkanRendererCreateInfo _rendererCreateInfo{ MaxFramesInFlight, _hardwareInfo, _renderSurface, _logicalDevice, _swapChain };
        std::unique_ptr<Mango::Renderer> _renderer = std::make_unique<Mango::VulkanRenderer>(false, _rendererCreateInfo);

        //Mango::RenderPassCreateInfo _renderPassCreateInfo{ VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };
        //Mango::RenderPass _renderPass{ _logicalDevice, _swapChain, _renderPassCreateInfo };
        //std::unique_ptr<Mango::DescriptorSetLayout> _globalLayout;
        //std::unique_ptr<Mango::DescriptorSetLayout> _textureLayout;
        //std::unique_ptr<Mango::DescriptorSetLayout> _drawLayout;
        //std::unique_ptr<Mango::UniformBuffersPool> _uniformBuffers;
        //Mango::FramebuffersPool _framebuffers{ _logicalDevice, _renderPass, _swapChain };
        //Mango::DescriptorPool _descriptorPool{ _poolSizes, _logicalDevice };
        //Mango::CommandPool _commandPool{ _logicalDevice, _queueFamilyIndices };
        //std::unique_ptr<Mango::GraphicsPipeline> _graphicsPipeline;
        //std::unique_ptr<Mango::CommandBuffersPool> _commandBuffers;

        //std::vector<VkSemaphore> _imageAvailableSemaphores{ MaxFramesInFlight };
        //std::vector<VkSemaphore> _renderFinishedSemaphores{ MaxFramesInFlight };
        //std::vector<VkFence> _inFlightFences{ MaxFramesInFlight };

        // ImGui pipeline
        //const std::vector<VkDescriptorPoolSize> _imGuiPoolSizes =
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

        //Mango::RenderPassCreateInfo _imGuiRenderPassCreateInfo{ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };
        //Mango::RenderPass _imGuiRenderPass{ _logicalDevice, _swapChain, _imGuiRenderPassCreateInfo };
        //Mango::FramebuffersPool _imGuiFramebuffers{ _logicalDevice, _imGuiRenderPass, _swapChain };
        //Mango::DescriptorPool _imGuiDescriptorPool{ _imGuiPoolSizes, _logicalDevice };
        //Mango::CommandPool _imGuiCommandPool{ _logicalDevice, _queueFamilyIndices };
        //Mango::CommandBuffersPool _imGuiCommandBuffers{ MaxFramesInFlight, _logicalDevice, _swapChain, _imGuiRenderPass, _imGuiCommandPool, *_graphicsPipeline };
    };

}
