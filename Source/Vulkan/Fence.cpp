#include "Fence.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

#include <string>

Mango::Fence::Fence(bool startSignaled, const Mango::LogicalDevice& logicalDevice) : _logicalDevice(logicalDevice.GetDevice())
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (startSignaled)
    {
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    auto createFenceResult = vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_fence);
    M_TRACE("Create fence result is: " + std::to_string(createFenceResult));
    M_ASSERT(createFenceResult == VK_SUCCESS && "Failed to create fence");
}

Mango::Fence::~Fence()
{
    vkDestroyFence(_logicalDevice, _fence, nullptr);
}

void Mango::Fence::WaitForFence()
{
    vkWaitForFences(_logicalDevice, 1, &_fence, VK_TRUE, UINT64_MAX);
}

void Mango::Fence::ResetFence()
{
    vkResetFences(_logicalDevice, 1, &_fence);
}
