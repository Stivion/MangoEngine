﻿#pragma once

#include "Windowing/Window.h"
#include "Render/RenderingLayer.h"
#include "Core/Scene.h"

#include "Platform/Windowing/GLFWWindow.h"
#include "Platform/Vulkan/Context.h"
#include "Platform/Vulkan/RenderingLayer_ImplVulkan.h"

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
        void InitializeWindow();
        void InitializeVulkan();
        void RunMainLoop();
        void DrawFrame();

    private:
        std::unique_ptr<Mango::GLFWWindow> _window;
        std::unique_ptr<const Mango::Context> _vulkanContext;
        std::unique_ptr<Mango::RenderingLayer_ImplVulkan> _renderingLayer;
        std::unique_ptr<Mango::Scene> _scene;
    };
}
