#pragma once

#include "../Windowing/Window.h"

#include <imgui.h>
#include <glm/glm.hpp>

namespace Mango
{
	class ImGuiEditor
	{
	public:
		ImGuiEditor(const Window* window);
		virtual ~ImGuiEditor();

		void ConstructEditor();
		const glm::vec2 GetViewportSize() const { return _viewportSize; };

		virtual void NewFrame(uint32_t currentFrame);
		virtual void EndFrame();

	protected:
		ImTextureID _viewportTextureId;
		glm::vec2 _viewportSize;

	};
}
