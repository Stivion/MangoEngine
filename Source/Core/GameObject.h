#pragma once

#include "GUID.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mango
{
	class GameObject
	{
	public:
		GameObject() = default;
		GameObject(Mango::GUID id);

		const void SetPosition(glm::vec3 position);

		const uint64_t GetId() const { return _id; }
		const uint64_t GetId() { return _id; }
		const glm::vec3 GetPosition() const { return _position; }
		const glm::vec3 GetPosition() { return _position; }

	private:
		const Mango::GUID _id;
		glm::vec3 _position;
	};
}
