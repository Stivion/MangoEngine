#include "Semaphore.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Semaphore::Semaphore(const Mango::LogicalDevice& logicalDevice) : _logicalDevice(logicalDevice.GetDevice())
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	auto createSemaphoreResult = vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_semaphore);
	M_TRACE("Create semaphore result is: " + std::to_string(createSemaphoreResult));
	M_ASSERT(createSemaphoreResult == VK_SUCCESS && "Failed to create semaphore");
}

Mango::Semaphore::~Semaphore()
{
	vkDestroySemaphore(_logicalDevice, _semaphore, nullptr);
}
