#pragma once

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
#include "Vulkan/CommandBuffer.h"

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
        void RunMainLoop();
        void DrawFrame();
        void FrameRender(ImDrawData* draw_data);
        void InitializeImGui();
        void CreateSyncObjects();
        
    private:
        // Windowing
        Mango::Window _window{ 1920, 1080 };

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
        Mango::CommandBuffer _commandBuffer{ _logicalDevice, _swapChain, _renderPass, _graphicsPipeline, _framebuffers, _commandPool };

        VkSemaphore _imageAvailableSemaphore;
        VkSemaphore _renderFinishedSemaphore;
        VkFence _inFlightFence;
        
        VkDescriptorPool _descriptorPool;
    };

}
