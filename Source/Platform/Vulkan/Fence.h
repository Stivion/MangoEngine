#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class Fence
	{
	public:
		Fence(bool startSignaled, const Mango::LogicalDevice& logicalDevice);
		Fence() = delete;
		Fence(const Fence&) = delete;
		Fence operator=(const Fence&) = delete;
		~Fence();

		void WaitForFence();
		void ResetFence();
		bool IsFenceSignaled(); // Signaled means it is waitable

		const VkFence& GetFence() const { return _fence; }

	private:
		VkFence _fence;
		const VkDevice& _logicalDevice;
	};
}
