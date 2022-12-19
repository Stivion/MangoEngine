﻿#pragma once

#include "Windowing/Window.h"
#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/RenderSurface.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Framebuffers.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/VertexBuffer.h"
#include "Vulkan/IndexBuffer.h"
#include "Vulkan/CommandBuffers.h"
#include "Vulkan/Vertex.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <cstdint>
#include <vector>

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
        const uint32_t MaxFramesInFlight = 2;

        void InitializeImGui();
        void CreateSyncObjects();
        void InitializeWindow();
        void InitializeVulkan();
        void RunMainLoop();
        void DrawFrame(uint32_t currentFrame, const Mango::VertexBuffer& vertexBuffer, const Mango::IndexBuffer& indexBuffer);
        void FrameRender(ImDrawData* draw_data);

        static void FramebufferResizedCallback(GLFWwindow* window, int width, int height);
        
    private:
        // Windowing
        Mango::Window _window{ 800, 600 };
        bool _framebufferResized = false;

        // Vulkan
        Mango::Instance _instance{};
        Mango::RenderSurface _renderSurface{ _window, _instance };
        Mango::PhysicalDevice _physicalDevice{ _instance, _renderSurface };
        Mango::QueueFamilyIndices _queueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        Mango::LogicalDevice _logicalDevice{ _physicalDevice, _queueFamilyIndices };
        Mango::SwapChainSupportDetails _swapChainSupportDetails = SwapChainSupportDetails::QuerySwapChainSupport(_physicalDevice.GetDevice(), _renderSurface.GetRenderSurface());
        Mango::SwapChain _swapChain{ _window, _renderSurface, _logicalDevice, _swapChainSupportDetails, _queueFamilyIndices };
        Mango::RenderPass _renderPass{ _logicalDevice, _swapChain };
        Mango::GraphicsPipeline _graphicsPipeline{ _logicalDevice, _swapChain, _renderPass, "vert.spv", "frag.spv" };
        Mango::Framebuffers _framebuffers{ _logicalDevice, _swapChain, _renderPass };
        Mango::CommandPool _commandPool{ _logicalDevice, _queueFamilyIndices };
        Mango::CommandBuffers _commandBuffers{ MaxFramesInFlight, _logicalDevice, _swapChain, _renderPass, _graphicsPipeline, _framebuffers, _commandPool };

        std::vector<VkSemaphore> _imageAvailableSemaphores{ MaxFramesInFlight };
        std::vector<VkSemaphore> _renderFinishedSemaphores{ MaxFramesInFlight };
        std::vector<VkFence> _inFlightFences{ MaxFramesInFlight };
        
        VkDescriptorPool _descriptorPool;
    };

}
