#pragma once

#include "Windowing/Window.h"
#include "Render/Renderer.h"
#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/RenderSurface.h"
#include "Vulkan/VulkanRenderer.h"
#include "GUI/Editor.h"

#include "Platform/Windowing/GLFWWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vector>
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

        static void FramebufferResizedCallback(Window* window, uint32_t width, uint32_t height);
        
    private:
        // Windowing
        std::unique_ptr<Mango::GLFWWindow> _window;
        bool _framebufferResized = false;

        // Vulkan
        std::unique_ptr<Mango::Instance> _instance;
        std::unique_ptr<Mango::RenderSurface> _renderSurface;
        std::unique_ptr<Mango::PhysicalDevice> _physicalDevice;
        std::unique_ptr<Mango::QueueFamilyIndices> _queueFamilyIndices;
        std::unique_ptr<Mango::LogicalDevice> _logicalDevice;
        std::unique_ptr<Mango::Renderer> _renderer;
        std::unique_ptr<Mango::Editor> _editor;

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
