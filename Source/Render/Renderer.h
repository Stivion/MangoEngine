#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mango
{
	class Renderer
	{
	public:
		// TODO: Old API, remove
		virtual void Draw() = 0;
		// End TODO

		virtual void DrawRect(glm::mat4 transform, glm::vec4 color) const = 0;
		virtual void DrawTriangle(glm::mat4 transform, glm::vec4 color) const = 0;

		virtual ~Renderer() = default;
	};
}
