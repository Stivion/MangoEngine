#pragma once

#include "../../Vulkan/RenderSurface.h" // TODO: Fix path after moving Vulkan under Platform
#include "../Windowing/GLFWWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Mango
{
	class RenderSurface_ImplGLFW : public RenderSurface
	{
	public:
		RenderSurface_ImplGLFW(const Mango::GLFWWindow& window, const Mango::Instance& instance);
		RenderSurface_ImplGLFW() = delete;
		RenderSurface_ImplGLFW(const RenderSurface_ImplGLFW&) = delete;
		RenderSurface_ImplGLFW operator=(const RenderSurface_ImplGLFW&) = delete;
	};
}
