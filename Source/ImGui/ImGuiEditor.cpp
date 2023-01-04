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
    NewFrame();
    
    // Here goes all editor UI code
    ImGui::ShowDemoWindow();
    //ImGui::Begin("Viewport");
    //ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    //const glm::vec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    //ImGui::Image(_viewportTextureId, ImVec2{ viewportSize.x, viewportSize.y });
    //_viewportTextureId - ImTextureId (VkDescriptorSet for Vulkan implementation)

    EndFrame();
}

void Mango::ImGuiEditor::NewFrame() const
{
    ImGui::NewFrame();
}

void Mango::ImGuiEditor::EndFrame() const
{
    ImGui::EndFrame();
}
