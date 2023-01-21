#include "ImGuiEditor.h"

Mango::ImGuiEditor::ImGuiEditor(const Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(window->GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(window->GetWindowHeight());
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
}

Mango::ImGuiEditor::~ImGuiEditor()
{
    ImGui::DestroyContext();
}

void Mango::ImGuiEditor::ConstructEditor()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    ImGui::SetWindowPos({ 0.0f, 0.0f });
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    auto dockSpaceId = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    _viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    ImGui::Image(_viewportTextureId, ImVec2{ _viewportSize.x, _viewportSize.y });
    ImGui::End();
}

void Mango::ImGuiEditor::NewFrame(uint32_t currentFrame)
{
    ImGui::NewFrame();
}

void Mango::ImGuiEditor::EndFrame()
{
    ImGui::EndFrame();
}
