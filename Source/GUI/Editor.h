#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "glm/glm.hpp"

namespace Mango
{
	class Editor
	{
	public:
		Editor();
		Editor(const Editor&) = delete;
		Editor operator=(const Editor&) = delete;
		~Editor();

		void ConstructEditor();
		void SetViewportTextureID(ImTextureID textureId);

	private:
		void InitializeImGui();

	private:
		ImTextureID _viewportTextureId;
	};
}
