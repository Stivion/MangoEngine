#pragma once

#include "CommandBuffer.h"

namespace Mango
{
	/// <summary>
	/// Interface for all classes, that could record Vulkan command buffer for submission
	/// </summary>
	class ICommandBufferRecorder
	{
	public:
		virtual const Mango::CommandBuffer& RecordCommandBuffer(uint32_t imageIndex) = 0;
	};
}
