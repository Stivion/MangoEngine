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
    // Main Mango Engine window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Mango Engine", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    ImGui::SetWindowPos({ 0.0f, 0.0f });
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    auto dockSpaceId = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar();

    // Viewport window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    // NOTE: Vulkan can't render to 0px screen so we force viewport to be at least 1px
    if (viewportPanelSize.x < 1.0f || viewportPanelSize.y < 1.0f)
    {
        ImGui::SetWindowSize({ 1.0f, 1.0f });
    }
    viewportPanelSize = ImGui::GetContentRegionAvail();
    _viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    ImGui::Image(_viewportTextureId, ImVec2{ _viewportSize.x, _viewportSize.y });
    ImGui::End();
    ImGui::PopStyleVar();

    // Entities window
    ImGui::Begin("Entities");
    for (auto [_, id, name] : _scene->GetRegistry().view<IdComponent, NameComponent>().each())
    {
        if (!_selectableEntities.contains(id.GetId()))
        {
            _selectableEntities[id.GetId()] = false;
        }
        ImGui::PushID(id.GetId());
        if (ImGui::Selectable(name.GetName(), _selectableEntities[id.GetId()]))
        {
            for (auto it = _selectableEntities.begin(); it != _selectableEntities.end(); it++)
            {
                it->second = false;
            }
            _selectableEntities[id.GetId()] = true;
        }

        // Single entity popup
        if (ImGui::BeginPopupContextItem("Single entity popup"))
        {
            if (ImGui::Button("Delete entity"))
            {
                _scene->DeleteEntity(id.GetId());
            }
            ImGui::EndPopup();
        }

        if (ImGui::IsWindowHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopupOnItemClick("Single entity popup");
        }

        ImGui::PopID();
    }

    // Entities popup
    if (ImGui::BeginPopup("Entities popup"))
    {
        if (ImGui::Button("Add triangle"))
        {
            _scene->AddTriangle();
        }

        if (ImGui::Button("Add rectangle"))
        {
            _scene->AddRectangle();
        }

        if (ImGui::Button("Add camera"))
        {
            _scene->AddCamera();
        }
        ImGui::EndPopup();
    }

    if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("Entities popup");
    }

    ImGui::End();

    // Entities component properties window
    ImGui::Begin("Properies");
    // TODO: Iteration over whole registry is not necessary here, we could just use selected entt:entity with registry.get<TComponent>(entity)
    for (auto [_, id, name, transform, color] : _scene->GetRegistry().view<IdComponent, NameComponent, TransformComponent, ColorComponent>().each())
    {
        if (id.GetId() != GetSelectedEntity())
        {
            continue;
        }

        ImGui::PushID(id.GetId());

        // NameComponent
        ImGui::InputText("Name", name.GetName(), name.GetBufferSize());

        // TransformComponent
        float transformDragSpeed = 0.1f;
        glm::vec3 translationVector = transform.GetTranslation();
        float inputTranslation[3] = { translationVector.x, translationVector.y, translationVector.z };
        if (ImGui::DragFloat3("Translation", inputTranslation, transformDragSpeed))
        {
            transform.SetTranslation({ inputTranslation[0], inputTranslation[1], inputTranslation[2] });
        }

        float rotation = transform.GetRotation();
        if (ImGui::DragFloat("Rotation", &rotation, transformDragSpeed, -360.0f, 360.0f))
        {
            transform.SetRotation(transform.GetRotationAxis(), rotation);
        }
        glm::vec3 rotationAxis = transform.GetRotationAxis();
        float inputRotationAxis[3] = { rotationAxis.x, rotationAxis.y, rotationAxis.z };
        if (ImGui::DragFloat3("Rotation Axis", inputRotationAxis, transformDragSpeed))
        {
            transform.SetRotation({ inputRotationAxis[0], inputRotationAxis[1], inputRotationAxis[2] }, transform.GetRotation());
        }

        glm::vec3 scaleVector = transform.GetScale();
        float inputScale[3] = { scaleVector.x, scaleVector.y, scaleVector.z };
        if (ImGui::DragFloat3("Scale", inputScale, transformDragSpeed))
        {
            transform.SetScale({ inputScale[0], inputScale[1], inputScale[2] });
        }

        // ColorComponent
        glm::vec4 colorVector = color.GetColor();
        float pickerColor[4] = { colorVector.r, colorVector.g, colorVector.b, colorVector.a };
        if (ImGui::ColorEdit4("Color", pickerColor))
        {
            color.SetColor({ pickerColor[0], pickerColor[1], pickerColor[2], pickerColor[3] });
        }

        ImGui::PopID();
    }

    // Camera properties
    for (auto [_, id, camera, transform, name] : _scene->GetRegistry().view<IdComponent, CameraComponent, TransformComponent, NameComponent>().each())
    {
        if (id.GetId() != GetSelectedEntity())
        {
            continue;
        }

        ImGui::PushID(id.GetId());

        // NameComponent
        ImGui::InputText("Name", name.GetName(), name.GetBufferSize());

        // TransformComponent
        float transformDragSpeed = 0.1f;
        glm::vec3 translationVector = transform.GetTranslation();
        float inputTranslation[3] = { translationVector.x, translationVector.y, translationVector.z };
        if (ImGui::DragFloat3("Translation", inputTranslation, transformDragSpeed))
        {
            transform.SetTranslation({ inputTranslation[0], inputTranslation[1], inputTranslation[2] });
        }

        // CameraComponent
        float cameraDragSpeed = 1.0f;
        float nearPlane = camera.GetNearPlane();
        if (ImGui::DragFloat("Near plane", &nearPlane, cameraDragSpeed))
        {
            camera.SetClippingPlanes(nearPlane, camera.GetFarPlane());
        }

        float farPlane = camera.GetFarPlane();
        if (ImGui::DragFloat("Far plane", &farPlane, cameraDragSpeed))
        {
            camera.SetClippingPlanes(camera.GetNearPlane(), farPlane);
        }

        float fov = camera.GetFOV();
        if (ImGui::DragFloat("FOV", &fov, cameraDragSpeed, -360.0f, 360.0f))
        {
            camera.SetFOV(fov);
        }

        float lookAtDragSpeed = 0.1f;
        glm::vec3 viewTargetVector = camera.GetViewTarget();
        float inputViewTarget[3] = { viewTargetVector.x, viewTargetVector.y, viewTargetVector.z };
        if (ImGui::DragFloat3("Look At", inputViewTarget, lookAtDragSpeed))
        {
            camera.SetViewTarget({ inputViewTarget[0], inputViewTarget[1], inputViewTarget[2] });
        }

        bool isPrimary = camera.IsPrimary();
        if (ImGui::Checkbox("Is Primary", &isPrimary))
        {
            for (auto [_, camera] : _scene->GetRegistry().view<CameraComponent>().each())
            {
                camera.SetPrimary(false);
            }
            camera.SetPrimary(isPrimary);
        }

        ImGui::PopID();
    }

    ImGui::End();

    // Assets window
    ImGui::Begin("Assets");
    // Assets placeholder
    ImGui::End();

    ImGui::ShowDemoWindow();
}

void Mango::ImGuiEditor::SetScene(Mango::Scene* scene)
{
    _scene = scene;
    _scene->OnCreate();
    _scene->AddCamera(); // Editor scene will always have an editor camera

    for (auto [entity, camera] : _scene->GetRegistry().view<CameraComponent>().each())
    {
        if (camera.IsEditorCamera())
        {
            camera.SetEditorCamera(true);
        }
    }
}

void Mango::ImGuiEditor::NewFrame(uint32_t currentFrame)
{
    ImGui::NewFrame();
}

void Mango::ImGuiEditor::EndFrame()
{
    ImGui::EndFrame();
}

inline Mango::GUID Mango::ImGuiEditor::GetSelectedEntity()
{
    for (auto it = _selectableEntities.begin(); it != _selectableEntities.end(); it++)
    {
        if (it->second == true)
        {
            return it->first;
        }
    }
    return Mango::GUID::Empty();
}
