#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mango
{
	class Renderer
	{
	public:
		virtual void DrawRect(glm::mat4 transform, glm::vec4 color) = 0;
		virtual void DrawTriangle(glm::mat4 transform, glm::vec4 color) = 0;

		virtual ~Renderer() = default;
	};
}
