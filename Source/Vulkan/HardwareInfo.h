#pragma once

#include "Instance.h"
#include "PhysicalDevice.h"
#include "QueueFamilyIndices.h"

namespace Mango
{
	struct HardwareInfo
	{
		Mango::Instance& Instance;
		Mango::PhysicalDevice& PhysicalDevice;
		Mango::QueueFamilyIndices& QueueFamilyIndices;
	};
}
