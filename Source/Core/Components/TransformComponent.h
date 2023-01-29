#pragma once

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mango
{
	class TransformComponent
	{
	public:
		TransformComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
		{
			_translation = translation;
			_rotation = rotation;
			_scale = scale;
		}

		inline glm::vec3 GetTranslation() { return _translation; }
		inline glm::vec3 GetRotation() { return _rotation; }
		inline glm::quat GetQuaternionRotation() { return glm::quat({ glm::radians(_rotation.x), glm::radians(_rotation.y), glm::radians(_rotation.z) }); }
		inline glm::vec3 GetScale() { return _scale; }

		void SetTranslation(glm::vec3 translation) { _translation = translation; }
		void SetRotation(glm::vec3 rotation) { _rotation = rotation; }
		void SetScale(glm::vec3 scale) { _scale = scale; }

		glm::mat4 GetTransform()
		{
			glm::mat4 rotation = glm::toMat4(GetQuaternionRotation());
			return glm::translate(glm::mat4(1.0f), _translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), _scale);
		}

	private:
		glm::vec3 _translation;
		glm::vec3 _scale;
		glm::vec3 _rotation;
	};
}
