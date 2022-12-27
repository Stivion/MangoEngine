#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class Fence
	{
	public:
		Fence(bool startSignaled, Mango::LogicalDevice& logicalDevice); // TODO: Make unique pointer
		Fence() = delete;
		Fence(const Fence&) = delete;
		Fence operator=(const Fence&) = delete;
		~Fence();

		void WaitForFence();
		void ResetFence();

	private:
		VkFence _fence;
		VkDevice& _logicalDevice;
	};
}
