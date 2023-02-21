#include "Application.h"

#include "Core/SceneManager.h"
#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

#include "Core/Scripting/ScriptEngine.h"

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();

    auto& renderer = _renderingLayer->GetRenderer();
    Mango::SceneManager::SetRenderer(&renderer);
    Mango::SceneManager::LoadEmpty();
    _renderingLayer->GetEditor().InitializeSceneForEditor();

    Mango::ScriptEngine scriptEngine;
}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::InitializeWindow()
{
    _window = std::make_unique<Mango::GLFWWindow>(1920, 1080);
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

    auto& scene = Mango::SceneManager::GetScene();
    
    scene.OnUpdate();
    // TODO: Should be independent from frame-rate
    scene.OnFixedUpdate();

    _renderingLayer->EndFrame();
}
