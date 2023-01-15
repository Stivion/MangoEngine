#pragma once

#include "../../Windowing/Window.h"
#include "Instance.h"
#include "RenderSurface.h"
#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"
#include "LogicalDevice.h"

#include <memory>

namespace Mango
{
	class Context
	{
	public:
		Context(Mango::Window* window);
		Context() = delete;
		Context(const Context&) = delete;
		Context operator=(const Context&) = delete;

		inline Mango::Window* GetWindow() const { return _window; }
		inline const Mango::Instance* GetInstance() const { return _instance.get(); }
		inline const Mango::RenderSurface* GetRenderSurface() const { return _renderSurface.get(); }
		inline const Mango::PhysicalDevice* GetPhysicalDevice() const { return _physicalDevice.get(); }
		inline const Mango::QueueFamilyIndices* GetQueueFamilyIndices() const { return _queueFamilyIndices.get(); }
		inline const Mango::LogicalDevice* GetLogicalDevice() const { return _logicalDevice.get(); }

	private:
		Window* _window;
		std::unique_ptr<Mango::Instance> _instance;
		std::unique_ptr<Mango::RenderSurface> _renderSurface;
		std::unique_ptr<Mango::PhysicalDevice> _physicalDevice;
		std::unique_ptr<Mango::QueueFamilyIndices> _queueFamilyIndices;
		std::unique_ptr<Mango::LogicalDevice> _logicalDevice;
	};
}
