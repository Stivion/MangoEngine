#pragma once

#include "GUID.h"
#include "Components/Components.h"
#include "../Render/Renderer.h"

#include <entt/entity/registry.hpp>
#include <box2d/b2_world.h>

namespace Mango
{
	enum SceneState
	{
		Play = 0,
		Stop = 1
	};

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

		// Start game view of this scene
		void OnPlay();

		// Stop game view of this scene
		void OnStop();

		inline Mango::SceneState GetSceneState() { return _sceneState; }

		// Add new triangle entity to scene
		void AddTriangle();

		// Add new rectangle entity to scene
		void AddRectangle();

		// Add new camera to scene
		entt::entity AddCamera();

		// Delete specified entity from scene
		void DeleteEntity(entt::entity entity);

	private:
		Mango::Renderer& _renderer;
		entt::registry _registry;
		Mango::SceneState _sceneState = Mango::SceneState::Stop;

	private:
		void AddDefaultEntity(Mango::GeometryType geometry);
		void SetRendererCamera(Mango::CameraComponent& camera, Mango::TransformComponent& transform);
	};
}
