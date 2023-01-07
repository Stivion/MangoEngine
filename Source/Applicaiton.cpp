#include "Application.h"

#include "Platform/Vulkan/RenderSurafce_ImplGLFW.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

bool Mango::Application::FramebufferResized = false;

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
    _window->SetFramebufferResizedCallback(FramebufferResizedCallback);
    _window->SetWindowUserPointer(this);
}

void Mango::Application::InitializeVulkan()
{
    _instance = std::make_unique<Mango::Instance>();
    _renderSurface = std::make_unique<Mango::RenderSurface_ImplGLFW>(*_window, *_instance);
    _physicalDevice = std::make_unique<Mango::PhysicalDevice>(*_instance, *_renderSurface);
    _queueFamilyIndices = std::make_unique<Mango::QueueFamilyIndices>(QueueFamilyIndices::FindQueueFamilies(_physicalDevice->GetDevice(), _renderSurface->GetRenderSurface()));
    _logicalDevice = std::make_unique<Mango::LogicalDevice>(*_physicalDevice, *_queueFamilyIndices);

    uint32_t width = _window->GetWindowWidth();
    uint32_t height = _window->GetWindowHeight();

    Mango::Renderer_ImplVulkan_CreateInfo rendererCreateInfo{};
    rendererCreateInfo.MaxFramesInFlight = MaxFramesInFlight;
    rendererCreateInfo.WindowFramebufferWidth = width;
    rendererCreateInfo.WindowFramebufferHeight = height;
    rendererCreateInfo.Instance = _instance.get();
    rendererCreateInfo.RenderSurface = _renderSurface.get();
    rendererCreateInfo.PhysicalDevice = _physicalDevice.get();
    rendererCreateInfo.QueueFamilyIndices = _queueFamilyIndices.get();
    rendererCreateInfo.LogicalDevice = _logicalDevice.get();
    _renderer = std::make_unique<Mango::Renderer_ImplVulkan>(rendererCreateInfo);
    _renderer->SetOnResizeCallback(FramebufferResizedCallback);

    const auto& currentExtent = _renderer->GetSwapChain()->GetSwapChainExtent();
    Mango::ViewportInfo viewportInfo{};
    viewportInfo.X = 0;
    viewportInfo.Y = 0;
    viewportInfo.Width = currentExtent.width;
    viewportInfo.Height = currentExtent.height;
    viewportInfo.MinDepth = 0;
    viewportInfo.MaxDepth = 1;

    Mango::ImGuiEditor_ImplGLFWVulkan_CreateInfo editorCreateInfo{};
    editorCreateInfo.MaxFramesInFlight = MaxFramesInFlight;
    editorCreateInfo.Window = _window.get();
    editorCreateInfo.Instance = _instance.get();
    editorCreateInfo.RenderSurface = _renderSurface.get();
    editorCreateInfo.PhysicalDevice = _physicalDevice.get();
    editorCreateInfo.QueueFamilyIndices = _queueFamilyIndices.get();
    editorCreateInfo.LogicalDevice = _logicalDevice.get();
    editorCreateInfo.SwapChain = _renderer->GetSwapChain();
    editorCreateInfo.GraphicsPipeline = _renderer->GetGraphicsPipeline();
    editorCreateInfo.ViewportInfo = viewportInfo;
    _editor = std::make_unique<Mango::ImGuiEditor_ImplGLFWVulkan>(editorCreateInfo);
}
void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    uint32_t currentFrame = 0;

    while (!_window->ShouldClose())
    {
        glfwPollEvents(); // TODO: WAT
        DrawFrame(currentFrame);
        currentFrame = (currentFrame + 1) % MaxFramesInFlight;
    }

    vkDeviceWaitIdle(_logicalDevice->GetDevice()); // TODO: Remove
}

/*
class Scene 
{

    void OnUpdate()
    {
        _renderer->DrawRect(transform, color);
        _renderer->DrawRect(transform, color);
        _renderer->DrawRect(transform, color);
        _renderer->DrawRect(transform, color);
        _renderer->DrawRect(transform, color);
        _renderer->DrawRect(transform, color);
    }

}
*/

void Mango::Application::DrawFrame(uint32_t currentFrame)
{
    _editor->NewFrame(currentFrame);
    _editor->ConstructEditor();
    const auto viewportSize = _editor->GetViewportSize();
    _editor->EndFrame();

    _renderer->BeginFrame(currentFrame);
    if (FramebufferResized)
    {
        _editor->HandleResize();
        FramebufferResized = false;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    _renderer->DrawTriangle(transform, { 1.0f, 1.0f, 0.0f, 1.0f });
    _editor->Draw();

    _renderer->EndFrame();
    
    _renderer->SubmitCommandBuffers({ _renderer->GetCurrentCommandBuffer(), _editor->GetCurrentCommandBuffer() });
    if (FramebufferResized)
    {
        _editor->HandleResize();
        FramebufferResized = false;
    }
}

void Mango::Application::FramebufferResizedCallback(Window* window, uint32_t width, uint32_t height)
{
    const auto& applicationPointer = reinterpret_cast<Application*>(window->GetWindowUserPointer());
    uint32_t currentWidth = window->GetWindowWidth();
    uint32_t currentHeight = window->GetWindowHeight();
    if (currentWidth == 0 || currentHeight == 0)
    {
        M_TRACE("Window is minimized.");
    }
    while (currentWidth == 0 || currentHeight == 0)
    {
        currentWidth = window->GetWindowWidth();
        currentHeight = window->GetWindowHeight();
    }
    applicationPointer->_renderer->HandleResize();
    applicationPointer->_editor->HandleResize();
    FramebufferResized = false;
}

void Mango::Application::FramebufferResizedCallback()
{
    FramebufferResized = true;
}
