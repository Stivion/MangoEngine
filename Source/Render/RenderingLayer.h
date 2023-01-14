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

	protected:
		uint32_t _currentFrame;
		uint32_t _maxFramesInFlight;
	};
}
