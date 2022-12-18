#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mango
{
	class Window
	{
	public:
		Window(uint32_t width, uint32_t height);
		Window() = delete;
		Window(const Window&) = delete;
		Window operator=(const Window&) = delete;
		~Window();

		GLFWwindow* GetWindow() const { return _window; }
		const uint32_t GetWindowWidth() const{ return _width; }
		const uint32_t GetWindowHeight() const { return _height; }
	private:
		GLFWwindow* _window;
		uint32_t _width;
		uint32_t _height;
	};
}
