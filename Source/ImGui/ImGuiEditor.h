#pragma once

#include "../Core/GUID.h"
#include "../Windowing/Window.h"
#include "../Core/SceneSerializer.h"

#include <imgui.h>
#include <glm/glm.hpp>

#include <string>

namespace Mango
{
	class ImGuiEditor
	{
	public:
		ImGuiEditor(const Window* window);
		virtual ~ImGuiEditor();

		void ConstructEditor();
		void InitializeSceneForEditor();
		const glm::vec2 GetViewportSize() const { return _viewportSize; };

		virtual void NewFrame(uint32_t currentFrame);
		virtual void EndFrame();

	protected:
		ImTextureID _viewportTextureId;
		glm::vec2 _viewportSize;

	private:
		entt::entity _selectedEntity;
		entt::entity _editorCamera;
		std::string _sceneState;
		
		bool _viewportCameraMoveStarted = false;
		ImVec2 _viewportCameraMoveStartMousePosition;

	private:
		inline float GetCameraRotationSpeed();
		inline float GetCameraMovementSpeed();
	};
}
