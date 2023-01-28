#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mango
{
	struct RendererCameraInfo
	{
		float NearPlane;
		float FarPlane;
		float FovDegrees;
		glm::vec3 ViewTarget;
		glm::vec3 Position;
	};

	class Renderer
	{
	public:
		virtual void DrawRect(glm::mat4 transform, glm::vec4 color) = 0;
		virtual void DrawTriangle(glm::mat4 transform, glm::vec4 color) = 0;

		virtual void SetCamera(RendererCameraInfo cameraInfo) = 0;

		virtual ~Renderer() = default;
	};
}
