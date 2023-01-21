#include "Application.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();

    auto& renderer = _renderingLayer->GetRenderer();
    _scene = std::make_unique<Mango::Scene>(renderer);
}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::InitializeWindow()
{
    _window = std::make_unique<Mango::GLFWWindow>(1200, 800);
}

void Mango::Application::InitializeVulkan()
{
    _vulkanContext = std::make_unique<const Mango::Context>(_window.get());
    _renderingLayer = std::make_unique<Mango::RenderingLayer_ImplVulkan>(_vulkanContext.get());
}

void Mango::Application::RunMainLoop()
{
    while (!_window->ShouldClose())
    {
        _window->PollEvents();
        DrawFrame();
    }

    _renderingLayer->WaitRenderingIdle();
}

void Mango::Application::DrawFrame()
{
    if (!_renderingLayer->BeginFrame())
    {
        return;
    }
    
    _scene->OnUpdate();

    _renderingLayer->EndFrame();
}
