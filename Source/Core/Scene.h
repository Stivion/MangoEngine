#pragma once

#include "GUID.h"
#include "Components/Components.h"
#include "../Render/Renderer.h"

#include <entt/entity/registry.hpp>

namespace Mango
{
	class Scene
	{
	public:
		Scene(Mango::Renderer& renderer);
		Scene(const Scene&) = delete;
		Scene operator=(const Scene&) = delete;

		entt::registry& GetRegistry() { return _registry; }

		// Happens when scene was loaded
		void OnCreate();

		// Happens every frame
		void OnUpdate();

		// Add new triangle entity to scene
		void AddTriangle();

		// Add new rectangle entity to scene
		void AddRectangle();

		// Add new camera to scene
		entt::entity AddCamera();

		// Delete specified entity from scene
		void DeleteEntity(Mango::GUID entityId);

	private:
		Mango::Renderer& _renderer;
		entt::registry _registry;

		// Editor camera
		RendererCameraInfo _editorCamera;
		glm::vec3 _editorCameraPosition;

	private:
		void AddDefaultEntity(Mango::GeometryType geometry);
		void SetRendererCamera(Mango::CameraComponent& camera, Mango::TransformComponent& transform);
	};
}
