#include "ImGuiEditor.h"

#include "../Infrastructure/IO/FileDialog.h"
#include "../Core/SceneManager.h"
#include "../Infrastructure/IO/FileWriter.h"
#include "../Infrastructure/IO/FileReader.h"

#include <filesystem>

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
	auto& scene = Mango::SceneManager::GetScene();

	// Main Mango Engine window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Mango Engine", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
				// Bug: Add entity, Play, Stop, Remove Entity, Play, Stop - Crash

				Mango::FileDialog fileDialog;
				std::filesystem::path filePath;
				const auto succeeded = fileDialog.Open(&filePath, { { L"JSON (*.json)", L"*.json" } });
				if (!succeeded)
				{
					goto cancelled;
				}

				auto sceneJson = Mango::FileReader::ReadAllText(filePath);
				Mango::SceneManager::LoadFromJson(sceneJson);
				InitializeSceneForEditor();
			}

			if (ImGui::MenuItem("Save as...", "Ctrl+Alt+S"))
			{
				Mango::FileDialog fileDialog;
				std::filesystem::path filePath;
				const auto succeeded = fileDialog.Save(&filePath, { { L"JSON (*.json)", L"*.json" } }, L"json");
				if (!succeeded)
				{
					goto cancelled;
				}

				Mango::SceneSerializer serializer;
				const auto serializedScene = serializer.Serialize(Mango::SceneManager::GetScene());

				// Open Save File Dialog here
				Mango::FileWriter::WriteFile(filePath, serializedScene);
			}

		cancelled:
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::SetWindowPos({ 0.0f, 0.0f });
	ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

	auto dockSpaceId = ImGui::GetID("EditorDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
	ImGui::PopStyleVar();

	// Viewport controls
	ImGuiWindowClass viewportControlsClass;
	viewportControlsClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
	ImGui::SetNextWindowClass(&viewportControlsClass);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("##ViewportControls", nullptr, ImGuiWindowFlags_NoCollapse);
	const char* playText = "Play";
	const char* stopText = "Stop";

	// Center buttons
	const float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonsWidth = 0.0f;
	buttonsWidth += ImGui::CalcTextSize(playText).x;
	buttonsWidth += ImGui::GetStyle().ItemInnerSpacing.x;
	buttonsWidth += ImGui::GetStyle().ItemSpacing.x;
	buttonsWidth += ImGui::CalcTextSize(stopText).x;
	buttonsWidth += ImGui::GetStyle().ItemInnerSpacing.x;
	float offset = windowWidth / 2.0 - buttonsWidth / 2.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

	float buttonsHeight = ImGui::GetWindowHeight();
	float firstButtonWidth = ImGui::CalcTextSize(playText).x;
	firstButtonWidth += ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;
	float secondButtonWidth = ImGui::CalcTextSize(stopText).x;
	secondButtonWidth += ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;

	if (ImGui::Button(playText, { firstButtonWidth, buttonsHeight }))
	{
		if (Mango::SceneManager::GetScene().GetSceneState() != Mango::SceneState::Play)
		{
			Mango::SceneSerializer serializer;
			_sceneState = serializer.Serialize(Mango::SceneManager::GetScene());
		}

		Mango::SceneManager::GetScene().OnPlay();
	}
	ImGui::SameLine();
	if (ImGui::Button(stopText, { secondButtonWidth, buttonsHeight }))
	{
		Mango::SceneManager::GetScene().OnStop();
		Mango::SceneManager::LoadFromJson(_sceneState);
		InitializeSceneForEditor();
	}
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

	// Moving camera
	if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right) && !_viewportCameraMoveStarted && Mango::SceneManager::GetScene().GetSceneState() != Mango::SceneState::Play)
	{
		_viewportCameraMoveStarted = true;
		_viewportCameraMoveStartMousePosition = ImGui::GetIO().MousePos;
	}

	if (_viewportCameraMoveStarted)
	{
		auto [editorCamera, editorCameraTransform] = Mango::SceneManager::GetScene().GetRegistry().get<CameraComponent, TransformComponent>(_editorCamera);

		// Camera look at controls
		auto mousePosition = ImGui::GetIO().MousePos;
		auto deltaX = _viewportCameraMoveStartMousePosition.x - mousePosition.x;
		auto deltaY = _viewportCameraMoveStartMousePosition.y - mousePosition.y;

		// Smooth camera movement
		deltaX *= GetCameraRotationSpeed();
		deltaY *= GetCameraRotationSpeed();

		auto currentRotation = editorCameraTransform.GetRotation();
		// X-mouse movement corresponds to pitch which is the second component
		// Y-mouse movement corresponds to yaw which is the first component
		editorCameraTransform.SetRotation({ currentRotation.x + deltaY, currentRotation.y + deltaX, currentRotation.z });
		_viewportCameraMoveStartMousePosition = mousePosition;

		// Camera position controls
		glm::vec3 newTranslation = editorCameraTransform.GetTranslation();

		if (ImGui::IsKeyDown(ImGuiKey_W))
		{
			glm::vec3 forwardVector = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 rotatedForwardVector = glm::rotate(editorCameraTransform.GetQuaternionRotation(), forwardVector);
			newTranslation -= rotatedForwardVector * GetCameraMovementSpeed();
		}

		if (ImGui::IsKeyDown(ImGuiKey_D))
		{
			glm::vec3 rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 rotatedRightVector = glm::rotate(editorCameraTransform.GetQuaternionRotation(), rightVector);
			newTranslation += rotatedRightVector * GetCameraMovementSpeed();
		}

		if (ImGui::IsKeyDown(ImGuiKey_A))
		{
			glm::vec3 leftVector = glm::vec3(-1.0f, 0.0f, 0.0f);
			glm::vec3 rotatedLeftVector = glm::rotate(editorCameraTransform.GetQuaternionRotation(), leftVector);
			newTranslation += rotatedLeftVector * GetCameraMovementSpeed();
		}

		if (ImGui::IsKeyDown(ImGuiKey_S))
		{
			glm::vec3 backwardVector = glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 rotatedBackwardVector = glm::rotate(editorCameraTransform.GetQuaternionRotation(), backwardVector);
			newTranslation -= rotatedBackwardVector * GetCameraMovementSpeed();
		}
		editorCameraTransform.SetTranslation(newTranslation);
	}

	if (_viewportCameraMoveStarted && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		_viewportCameraMoveStarted = false;
	}

	ImGui::End();
	ImGui::PopStyleVar();

	// Entities window
	ImGui::Begin("Entities");
	for (auto [entity, id, name] : Mango::SceneManager::GetScene().GetRegistry().view<IdComponent, NameComponent>().each())
	{
		ImGui::PushID(id.GetId());
		auto entitySelected = entity == _selectedEntity;
		if (ImGui::Selectable(name.GetName(), entitySelected))
		{
			_selectedEntity = entity;
		}

		// Single entity popup
		if (ImGui::BeginPopupContextItem("Single entity popup"))
		{
			if (entity != _editorCamera)
			{
				if (ImGui::Button("Delete entity"))
				{
					Mango::SceneManager::GetScene().DeleteEntity(entity);
				}
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
			Mango::SceneManager::GetScene().AddTriangle();
		}

		if (ImGui::Button("Add rectangle"))
		{
			Mango::SceneManager::GetScene().AddRectangle();
		}

		if (ImGui::Button("Add camera"))
		{
			Mango::SceneManager::GetScene().AddCamera();
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
	if (Mango::SceneManager::GetScene().GetRegistry().valid(_selectedEntity))
	{
		auto [id, name, transform] = Mango::SceneManager::GetScene().GetRegistry().get<IdComponent, NameComponent, TransformComponent>(_selectedEntity);
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

		glm::vec3 rotationVector = transform.GetRotation();
		float inputRotation = rotationVector.z;
		if (ImGui::DragFloat("Rotation", &inputRotation, transformDragSpeed))
		{
			transform.SetRotation({ rotationVector.x, rotationVector.y, inputRotation });
		}

		glm::vec3 scaleVector = transform.GetScale();
		float inputScale[2] = { scaleVector.x, scaleVector.y };
		if (ImGui::DragFloat2("Scale", inputScale, transformDragSpeed))
		{
			transform.SetScale({ inputScale[0], inputScale[1], scaleVector.z });
		}

		// ColorComponent
		auto color = Mango::SceneManager::GetScene().GetRegistry().try_get<ColorComponent>(_selectedEntity);
		if (color != nullptr)
		{
			glm::vec4 colorVector = color->GetColor();
			float pickerColor[4] = { colorVector.r, colorVector.g, colorVector.b, colorVector.a };
			if (ImGui::ColorEdit4("Color", pickerColor))
			{
				color->SetColor({ pickerColor[0], pickerColor[1], pickerColor[2], pickerColor[3] });
			}
		}

		// CameraComponent
		auto camera = Mango::SceneManager::GetScene().GetRegistry().try_get<CameraComponent>(_selectedEntity);
		if (camera != nullptr)
		{
			float cameraDragSpeed = 1.0f;
			float nearPlane = camera->GetNearPlane();
			if (ImGui::DragFloat("Near plane", &nearPlane, cameraDragSpeed))
			{
				camera->SetClippingPlanes(nearPlane, camera->GetFarPlane());
			}

			float farPlane = camera->GetFarPlane();
			if (ImGui::DragFloat("Far plane", &farPlane, cameraDragSpeed))
			{
				camera->SetClippingPlanes(camera->GetNearPlane(), farPlane);
			}

			float fov = camera->GetFOV();
			if (ImGui::DragFloat("FOV", &fov, cameraDragSpeed, -360.0f, 360.0f))
			{
				camera->SetFOV(fov);
			}

			bool isPrimary = camera->IsPrimary();
			if (ImGui::Checkbox("Is Primary", &isPrimary))
			{
				for (auto [_, otherCamera] : Mango::SceneManager::GetScene().GetRegistry().view<CameraComponent>().each())
				{
					otherCamera.SetPrimary(false);
				}
				camera->SetPrimary(isPrimary);
			}
		}

		// Rigidbody component
		auto rigidbody = Mango::SceneManager::GetScene().GetRegistry().try_get<RigidbodyComponent>(_selectedEntity);
		if (rigidbody != nullptr)
		{
			bool isDynamic = rigidbody->IsDynamic();
			if (ImGui::Checkbox("Is Dynamic", &isDynamic))
			{
				rigidbody->SetDynamic(isDynamic);
			}
		}

		// Script component
		auto script = Mango::SceneManager::GetScene().GetRegistry().try_get<ScriptComponent>(_selectedEntity);
		if (script != nullptr)
		{
			ImGui::InputText("Script", script->GetFileName(), script->GetBufferSize());
		}

		ImGui::PopID();

		// Components popup
		if (ImGui::BeginPopup("Components popup"))
		{
			auto rigidbodyExist = Mango::SceneManager::GetScene().GetRegistry().try_get<RigidbodyComponent>(_selectedEntity) != nullptr;
			if (!rigidbodyExist && ImGui::Button("Add rigidbody"))
			{
				Mango::SceneManager::GetScene().AddRigidbody(_selectedEntity);
			}

			auto scriptExist = Mango::SceneManager::GetScene().GetRegistry().try_get<ScriptComponent>(_selectedEntity) != nullptr;
			if (!scriptExist && ImGui::Button("Add script"))
			{
				Mango::SceneManager::GetScene().AddScript(_selectedEntity);
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("Components popup");
		}
	}
	ImGui::End();

	// Assets window
	ImGui::Begin("Assets");
	// Assets placeholder
	ImGui::End();

	ImGui::ShowDemoWindow();
}

void Mango::ImGuiEditor::NewFrame(uint32_t currentFrame)
{
	ImGui::NewFrame();
}

void Mango::ImGuiEditor::EndFrame()
{
	ImGui::EndFrame();
}

inline float Mango::ImGuiEditor::GetCameraRotationSpeed()
{
	return 0.1f;
}

inline float Mango::ImGuiEditor::GetCameraMovementSpeed()
{
	return 0.01f;
}

void Mango::ImGuiEditor::InitializeSceneForEditor()
{
	bool editorCameraExist = false;
	for (const auto& [entity, camera] : Mango::SceneManager::GetScene().GetRegistry().view<CameraComponent>().each())
	{
		if (camera.IsEditorCamera())
		{
			_editorCamera = entity;
			editorCameraExist = true;
			break;
		}
	}

	if (!editorCameraExist)
	{
		_editorCamera = Mango::SceneManager::GetScene().AddCamera(); // Editor scene will always have an editor camera
		auto [id, camera, transform, name] = Mango::SceneManager::GetScene().GetRegistry().get<IdComponent, CameraComponent, TransformComponent, NameComponent>(_editorCamera);
		camera.SetEditorCamera(true);
		camera.SetPrimary(true);
		name.SetName("Editor Camera");
		auto currentTranslation = transform.GetTranslation();
		transform.SetTranslation({ currentTranslation.x, currentTranslation.y, 5.0f });
		transform.SetRotation({ 0.0f, 0.0f, 0.0f });
	}
	Mango::SceneManager::GetScene().OnCreate();
}
