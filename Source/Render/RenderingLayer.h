#pragma once

#include <cstdint>

namespace Mango
{
	class RenderingLayer
	{
	public:
		typedef void (*FramebufferResizedCallback)(void* userPointer);

		// Begin new frame. Keeps frame count internally
		virtual bool BeginFrame() = 0;

		// End frame and present to screen
		virtual bool EndFrame() = 0;

	protected:
		uint32_t _currentFrame;
		uint32_t _maxFramesInFlight;
	};
}
