#pragma once

#include "CommandBuffer.h"

namespace Mango
{
	class CommandBufferRecorder
	{
	public:
		virtual const void RecordCommandBuffer(
			const Mango::CommandBuffer* commandBuffer,
			const Mango::Framebuffer* framebuffer
		) const = 0;
	};
}
