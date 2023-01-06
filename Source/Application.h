#pragma once

#include "Windowing/Window.h"
#include "Render/Renderer.h"
#include "ImGui/ImGuiEditor.h"

#include "Platform/Windowing/GLFWWindow.h"
#include "Platform/ImGui/ImGuiEditor_ImplGLFWVulkan.h"
#include "Platform/Vulkan/Instance.h"
#include "Platform/Vulkan/PhysicalDevice.h"
#include "Platform/Vulkan/LogicalDevice.h"
#include "Platform/Vulkan/RenderSurface.h"
#include "Platform/Vulkan/Renderer_ImplVulkan.h"

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
        
        void Run();

    private:
        const static uint32_t MaxFramesInFlight = 1;

        void InitializeWindow();
        void InitializeVulkan();
        void RunMainLoop();
        void DrawFrame(uint32_t currentFrame);

        static void FramebufferResizedCallback(Window* window, uint32_t width, uint32_t height);
        static void FramebufferResizedCallback();
        
    private:
        // Windowing
        std::unique_ptr<Mango::GLFWWindow> _window;
        static bool FramebufferResized;

        // Vulkan
        std::unique_ptr<Mango::Instance> _instance;
        std::unique_ptr<Mango::RenderSurface> _renderSurface;
        std::unique_ptr<Mango::PhysicalDevice> _physicalDevice;
        std::unique_ptr<Mango::QueueFamilyIndices> _queueFamilyIndices;
        std::unique_ptr<Mango::LogicalDevice> _logicalDevice;
        std::unique_ptr<Mango::Renderer_ImplVulkan> _renderer;

        // Editor
        std::unique_ptr<Mango::ImGuiEditor_ImplGLFWVulkan> _editor;
    };
}
