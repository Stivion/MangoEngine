#pragma once

#include <glm/glm.hpp>

namespace Mango
{
	// TODO: This hardcoded structs could be any data that we want to pass to shaders
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 View;
		alignas(16) glm::mat4 Projection;
	};

	struct DynamicUniformBufferObject
	{
		alignas(16) glm::mat4 Model;
	};
}
