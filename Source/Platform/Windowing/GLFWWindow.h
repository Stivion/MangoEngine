#pragma once

#include "../../Windowing/Window.h"

#include <GLFW/glfw3.h>

namespace Mango
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(uint32_t width, uint32_t height);
		~GLFWWindow();

		bool ShouldClose() override;
		void SetFramebufferResizedCallback(FramebufferResizedCallback callback) override;

		GLFWwindow* GetGLFWWindow() const { return _window; }

	private:
		GLFWwindow* _window;

	private:
		static void GLFWWindowFramebufferResizedCallback(GLFWwindow* window, int width, int height);
	};
}
