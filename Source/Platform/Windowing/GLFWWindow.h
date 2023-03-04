#pragma once

#include "../../Core/Input.h"
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
		void PollEvents() override;
		void SetFramebufferResizedCallback(FramebufferResizedCallback callback) override;

		GLFWwindow* GetGLFWWindow() const { return _window; }
		const uint32_t GetWindowWidth() const override;
		const uint32_t GetWindowHeight() const override;

	private:
		GLFWwindow* _window;

	private:
		static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void GLFWCursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
		static void GLFWWindowFramebufferResizedCallback(GLFWwindow* window, int width, int height);
	};
}
