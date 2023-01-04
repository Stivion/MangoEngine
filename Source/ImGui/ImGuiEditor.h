#pragma once

#include "../Windowing/Window.h"

#include "imgui.h"

namespace Mango
{
	class ImGuiEditor
	{
	public:
		ImGuiEditor(const Window* window);
		virtual ~ImGuiEditor();

		void ConstructEditor();

		virtual void NewFrame() const;
		virtual void EndFrame() const;
		virtual void Draw() const = 0;

	protected:
		ImTextureID _viewportTextureId;

	};
}
