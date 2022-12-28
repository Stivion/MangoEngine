#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Mango
{
	class Semaphore
	{
	public:
		Semaphore(const Mango::LogicalDevice& logicalDevice);
		Semaphore() = delete;
		Semaphore(const Semaphore&) = delete;
		Semaphore operator=(const Semaphore&) = delete;
		~Semaphore();

		VkSemaphore& GetSemaphore() { return _semaphore; }

	private:
		VkSemaphore _semaphore;
		const VkDevice& _logicalDevice;
	};
}
