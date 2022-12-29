#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mango
{
	class Window
	{
	public:
		typedef void (*FramebufferResizedCallback)(GLFWwindow* window, int width, int height);

		Window(uint32_t width, uint32_t height);
		Window() = delete;
		Window(const Window&) = delete;
		Window operator=(const Window&) = delete;
		~Window();

		void SetFramebufferResizedCallback(FramebufferResizedCallback callback);
		void SetWindowUserPointer(void* pointer);

		GLFWwindow* GetWindow() const { return _window; }
		const uint32_t GetWindowWidth() const{ return _width; }
		const uint32_t GetWindowHeight() const { return _height; }
	private:
		GLFWwindow* _window;
		uint32_t _width;
		uint32_t _height;

	private:
		FramebufferResizedCallback _framebufferResizedCallback = nullptr;
	};
}
