#pragma once

#include <glm/glm.hpp>

namespace Mango
{
	struct UniformBufferObject
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}
