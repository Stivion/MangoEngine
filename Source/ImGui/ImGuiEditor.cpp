#include "ImGuiEditor.h"

Mango::ImGuiEditor::ImGuiEditor(const Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = static_cast<float>(window->GetWindowWidth());
    io.DisplaySize.y = static_cast<float>(window->GetWindowHeight());
    ImGui::StyleColorsDark();
}

Mango::ImGuiEditor::~ImGuiEditor()
{
    ImGui::DestroyContext();
}

void Mango::ImGuiEditor::ConstructEditor()
{
    ImGui::Begin("Viewport");
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
