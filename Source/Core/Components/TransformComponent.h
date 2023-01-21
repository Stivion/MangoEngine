#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mango
{
	class TransformComponent
	{
	public:
		TransformComponent(glm::vec3 translation, glm::vec3 rotationAxis, float rotation, glm::vec3 scale)
		{
			_translation = translation;
			_rotationAxis = rotationAxis;
			_rotation = rotation;
			_scale = scale;
		}

		inline glm::vec3 GetTranslation() { return _translation; }
		inline glm::vec3 GetRotationAxis() { return _rotationAxis; }
		inline float GetRotation() { return _rotation; }
		inline glm::vec3 GetScale() { return _scale; }

		void SetTranslation(glm::vec3 translation) { _translation = translation; }
		void SetRotation(glm::vec3 rotationAxis, float rotation) { _rotationAxis = rotationAxis; _rotation = rotation; }
		void SetScale(glm::vec3 scale) { _scale = scale; }

		glm::mat4 GetTransform()
		{
			return glm::translate(glm::mat4(1.0f), _translation)
				* glm::rotate(glm::mat4(1.0f), _rotation, _rotationAxis)
				* glm::scale(glm::mat4(1.0f), _scale);
		}

	private:
		glm::vec3 _translation;
		glm::vec3 _rotationAxis;
		float _rotation;
		glm::vec3 _scale;
	};
}
