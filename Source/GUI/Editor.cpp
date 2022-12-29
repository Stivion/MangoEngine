#include "Editor.h"

Mango::Editor::Editor()
{
}

Mango::Editor::~Editor()
{
}

void Mango::Editor::ConstructEditor()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    const glm::vec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    ImGui::Image(_viewportTextureId, ImVec2{ viewportSize.x, viewportSize.y });

    ImGui::End();
}

void Mango::Editor::SetViewportTextureID(ImTextureID textureId)
{
    _viewportTextureId = textureId;
}

void Mango::Editor::InitializeImGui()
{
    
}
