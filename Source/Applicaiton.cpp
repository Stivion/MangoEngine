#include "Application.h"

#include "Platform/Vulkan/RenderSurafce_ImplGLFW.h"

#include "Infrastructure/Assert/Assert.h"
#include "Infrastructure/Logging/Logging.h"

#include <chrono>

Mango::Application::Application()
{
    InitializeWindow();
    InitializeImGui();
    InitializeVulkan();
}

Mango::Application::~Application()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Mango::Application::Run()
{
    RunMainLoop();
}

void Mango::Application::InitializeWindow()
{
    _window = std::make_unique<Mango::GLFWWindow>(800, 600);
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

    Mango::VulkanRendererCreateInfo rendererCreateInfo{};
    rendererCreateInfo.MaxFramesInFlight = MaxFramesInFlight;
    rendererCreateInfo.WindowFramebufferWidth = width;
    rendererCreateInfo.WindowFramebufferHeight = height;
    rendererCreateInfo.Instance = _instance.get();
    rendererCreateInfo.RenderSurface = _renderSurface.get();
    rendererCreateInfo.PhysicalDevice = _physicalDevice.get();
    rendererCreateInfo.QueueFamilyIndices = _queueFamilyIndices.get();
    rendererCreateInfo.LogicalDevice = _logicalDevice.get();
    _renderer = std::make_unique<Mango::VulkanRenderer>(rendererCreateInfo);
    _editor->SetViewportTextureID(_renderer->GetViewportTextureId());
}

void Mango::Application::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(_window->GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(_window->GetWindowHeight());
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(_window->GetGLFWWindow(), true);
    _editor = std::make_unique<Mango::Editor>();
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

    vkDeviceWaitIdle(_logicalDevice->GetDevice());
}

void Mango::Application::DrawFrame(uint32_t currentFrame)
{
    _editor->ConstructEditor();
    _renderer->Draw();
}

void Mango::Application::FramebufferResizedCallback(Window* window, uint32_t width, uint32_t height)
{
    const auto& applicationPointer = reinterpret_cast<Application*>(window->GetWindowUserPointer());
    // TODO: Fix me
    //int width = 0, height = 0;
    //glfwGetFramebufferSize(window, &width, &height);
    //if (width == 0 || height == 0)
    //{
    //    M_TRACE("Window is minimized.");
    //}
    //while (width == 0 || height == 0)
    //{
    //    glfwGetFramebufferSize(window, &width, &height);
    //    glfwWaitEvents();
    //}
    //applicationPointer->_renderer->HandleFramebuffersResized();
    applicationPointer->_framebufferResized = true;
}
