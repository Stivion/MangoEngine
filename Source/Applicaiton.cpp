#include "Application.h"

#include "Core/SceneManager.h"
#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

const uint32_t _fixedUpdatePerSecond = 60;
const uint32_t _fixedUpdateStepMillisecond = std::ceil((1.0f / _fixedUpdatePerSecond) * 1000);

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();

    auto& renderer = _renderingLayer->GetRenderer();
    Mango::SceneManager::SetRenderer(&renderer);
    Mango::SceneManager::LoadEmpty();
    _renderingLayer->GetEditor().InitializeSceneForEditor();
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
    
    auto currentTime = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _lastOnFixedUpdate);
    if (timeDiff.count() > _fixedUpdateStepMillisecond)
    {
        scene.OnFixedUpdate();
        _lastOnFixedUpdate = std::chrono::steady_clock::now();
    }

    _renderingLayer->EndFrame();
}
