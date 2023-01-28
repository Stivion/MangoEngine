#pragma once

#include "../Core/GUID.h"
#include "../Windowing/Window.h"
#include "../Core/Scene.h"

#include <imgui.h>
#include <glm/glm.hpp>

#include <map>

namespace Mango
{
	class ImGuiEditor
	{
	public:
		ImGuiEditor(const Window* window);
		virtual ~ImGuiEditor();

		void ConstructEditor();
		const glm::vec2 GetViewportSize() const { return _viewportSize; };
		void SetScene(Mango::Scene* scene);

		virtual void NewFrame(uint32_t currentFrame);
		virtual void EndFrame();

	protected:
		ImTextureID _viewportTextureId;
		glm::vec2 _viewportSize;

	private:
		Mango::Scene* _scene;
		std::map<Mango::GUID, bool> _selectableEntities;
		entt::entity _editorCamera;
		
		bool _viewportCameraMoveStarted = false;
		ImVec2 _viewportCameraMoveStartMousePosition;

	private:
		inline Mango::GUID GetSelectedEntity();

	};
}
