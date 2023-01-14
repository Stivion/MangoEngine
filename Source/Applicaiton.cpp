#include "Application.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

Mango::Application::Application()
{
    InitializeWindow();
    InitializeVulkan();
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
        glfwPollEvents(); // TODO: WAT
        DrawFrame();
    }

    vkDeviceWaitIdle(_vulkanContext->GetLogicalDevice()->GetDevice()); // TODO: Remove
}

void Mango::Application::DrawFrame()
{
    _renderingLayer->BeginFrame();

    auto& renderer = _renderingLayer->GetRenderer();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    renderer.DrawTriangle(transform, { 1.0f, 1.0f, 0.0f, 1.0f });
    //transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
    //    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    //    * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    //_renderer->DrawTriangle(transform, { 0.0f, 1.0f, 1.0f, 1.0f });

    _renderingLayer->EndFrame();
}
