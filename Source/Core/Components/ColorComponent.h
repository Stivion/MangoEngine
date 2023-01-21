#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mango
{
	class ColorComponent
	{
	public:
		ColorComponent(glm::vec4 color) { _color = color; }

		inline glm::vec4 GetColor() const { return _color; }
		void SetColor(glm::vec4 color) { _color = color; }

	private:
		glm::vec4 _color;
	};
}
