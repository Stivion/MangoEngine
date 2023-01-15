#pragma once

#include "LogicalDevice.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "RenderArea.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Mango
{
	class FramebuffersPool
	{
	public:
        FramebuffersPool(
            const Mango::LogicalDevice& logicalDevice,
            const Mango::RenderPass& renderPass,
            const Mango::RenderArea& renderArea,
            const Mango::RenderAreaInfo& renderAreaInfo
        );
        FramebuffersPool() = delete;
        FramebuffersPool(const FramebuffersPool&) = delete;
        FramebuffersPool operator=(const FramebuffersPool&) = delete;
        ~FramebuffersPool();

        Mango::Framebuffer& GetFramebuffer(uint32_t index) const { return *_framebuffers[index]; }
        const uint32_t GetFramebuffersCount() const { return _framebuffersCount; }

    private:
        std::vector<Mango::Framebuffer*> _framebuffers;
        const uint32_t _framebuffersCount;
	};
}