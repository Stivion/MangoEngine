#include "Application.h"

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
    _window.SetFramebufferResizedCallback(FramebufferResizedCallback);
    _window.SetWindowUserPointer(this);
}

void Mango::Application::InitializeVulkan()
{
    _instance = std::make_unique<Mango::Instance>();
    _renderSurface = std::make_unique<Mango::RenderSurface>(_window, *_instance);
    _physicalDevice = std::make_unique<Mango::PhysicalDevice>(*_instance, *_renderSurface);
    _queueFamilyIndices = std::make_unique<Mango::QueueFamilyIndices>(QueueFamilyIndices::FindQueueFamilies(_physicalDevice->GetDevice(), _renderSurface->GetRenderSurface()));
    _logicalDevice = std::make_unique<Mango::LogicalDevice>(*_physicalDevice, *_queueFamilyIndices);
    
    int width, height;
    glfwGetFramebufferSize(_window.GetWindow(), &width, &height);

    Mango::VulkanRendererCreateInfo rendererCreateInfo{};
    rendererCreateInfo.MaxFramesInFlight = MaxFramesInFlight;
    rendererCreateInfo.WindowFramebufferWidth = static_cast<uint32_t>(width);
    rendererCreateInfo.WindowFramebufferHeight = static_cast<uint32_t>(height);
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
    io.DisplaySize.x = static_cast<float>(_window.GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(_window.GetWindowHeight());
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(_window.GetWindow(), true);
    _editor = std::make_unique<Mango::Editor>();
}

void Mango::Application::RunMainLoop()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    uint32_t currentFrame = 0;

    while (!glfwWindowShouldClose(_window.GetWindow()))
    {
        glfwPollEvents();
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

void Mango::Application::FramebufferResizedCallback(GLFWwindow* window, int width, int height)
{
    auto applicationPointer = reinterpret_cast<Mango::Application*>(glfwGetWindowUserPointer(window));
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
