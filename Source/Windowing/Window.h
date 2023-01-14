#pragma once

#include <cstdint>

namespace Mango
{
	class Window
	{
	public:
		typedef void (*FramebufferResizedCallback)(Window* window, uint32_t width, uint32_t height);

		Window(uint32_t width, uint32_t height) : _width(width), _height(height) {};
		Window() = delete;
		Window(const Window&) = delete;
		virtual ~Window() = default;

		virtual bool ShouldClose() = 0;
		virtual void PollEvents() = 0;

		virtual void SetFramebufferResizedCallback(FramebufferResizedCallback callback) = 0;
		virtual void SetWindowUserPointer(void* userData) { _userData = userData; };

		virtual void* GetWindowUserPointer() { return _userData; }
		virtual const uint32_t GetWindowWidth() const{ return _width; }
		virtual const uint32_t GetWindowHeight() const { return _height; }

	protected:
		uint32_t _width;
		uint32_t _height;
		void* _userData;
		FramebufferResizedCallback _framebufferResizedCallback = nullptr;
	};
}
