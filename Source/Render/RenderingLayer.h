#pragma once

#include <cstdint>

namespace Mango
{
	class RenderingLayer
	{
	public:
		typedef void (*FramebufferResizedCallback)(void* userPointer);

		// Begin new frame. Keeps frame count internally
		virtual void BeginFrame() = 0;

		// End frame and present to screen
		virtual void EndFrame() = 0;

		// Set framebuffer callback, that will be called every time when resize is detected internally or extenally
		void SetFramebufferResizedCallback(FramebufferResizedCallback callback) { _framebufferResizedCallback = callback; }

		// Set user defined pointer, that will be passed as a parameter in FramebufferResizedCallback
		void SetUserDefinedCallbackPointer(void* userPointer) { _userPointer = userPointer; }

	protected:
		uint32_t _currentFrame;
		uint32_t _maxFramesInFlight;
		FramebufferResizedCallback _framebufferResizedCallback;
		void* _userPointer;
	};
}
