﻿#pragma once

#include "Windowing/Window.h"
#include "Render/Renderer.h"
#include "Render/RenderingLayer.h"
#include "ImGui/ImGuiEditor.h"

#include "Platform/Windowing/GLFWWindow.h"
#include "Platform/ImGui/ImGuiEditor_ImplGLFWVulkan.h"
#include "Platform/Vulkan/Context.h"
#include "Platform/Vulkan/Renderer_ImplVulkan.h"
#include "Platform/Vulkan/RenderingLayer_ImplVulkan.h"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

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
        void InitializeWindow();
        void InitializeVulkan();
        void RunMainLoop();
        void DrawFrame();

        static void FramebufferResizedCallback(void* applicationPtr);

        void InitializeViewportImage(uint32_t width, uint32_t height);
        void InitializeViewportImageView();
        
    private:
        // Windowing
        std::unique_ptr<Mango::GLFWWindow> _window;

        // Vulkan
        std::unique_ptr<const Mango::Context> _vulkanContext;
        std::unique_ptr<Mango::Renderer_ImplVulkan> _renderer;
        std::unique_ptr<Mango::RenderingLayer_ImplVulkan> _renderingLayer;
        Mango::RenderArea _windowRenderArea;
        Mango::RenderArea _viewportRenderArea;
        Mango::RenderAreaInfo _renderAreaInfo;
        Mango::RenderAreaInfo _viewportRenderAreaInfo;

        // Editor
        std::unique_ptr<Mango::ImGuiEditor_ImplGLFWVulkan> _editor;
    };
}
