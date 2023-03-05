#pragma once

#include "GUID.h"
#include "Components/Components.h"
#include "../Render/Renderer.h"
#include "Scripting/ScriptEngine.h"
#include "Input.h"

#include <entt/entity/registry.hpp>
#include <box2d/box2d.h>

#include <memory>

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
		~Scene();

		entt::registry& GetRegistry() { return _registry; }

		// Happens when scene was loaded
		void OnCreate();

		// Happens every frame
		void OnUpdate();

		// Happens 60 times per frame
		void OnFixedUpdate();

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

		void AddRigidbody(entt::entity entity);
		void AddScript(entt::entity entity);

		// Delete specified entity from scene
		void DeleteEntity(entt::entity entity);

	private:
		// Manipualte scene entities methods
		static void ApplyForce(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 force);
		static glm::vec2 GetPosition(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId);
		static void SetPosition(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 position);
		static bool GetKeyPressed(Mango::ScriptEngine* scriptEngine, Mango::Key key);
		static bool GetMouseButtonPressed(Mango::ScriptEngine* scriptEngine, Mango::MouseButton mouseButton);
		static glm::vec2 GetMouseCursorPosition(Mango::ScriptEngine* scriptEngine);
		static float GetRotation(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId);
		static void SetRotation(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, float rotation);
		static glm::vec2 GetScale(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId);
		static void SetScale(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 scale);

	private:
		Mango::Renderer& _renderer;
		entt::registry _registry;
		Mango::SceneState _sceneState = Mango::SceneState::Stop;
		
		// Physics
		const float _timeStep = 1.0f / 60.0f;
		const int32_t _velocityIterations = 8;
		const int32_t _positionIterations = 3;
		b2World _physicsWorld{{ 0.0f, -9.8f }};

		// Scripting
		std::unique_ptr<Mango::ScriptEngine> _scriptEngine;

	private:
		void AddDefaultEntity(Mango::GeometryType geometry);
		void SetRendererCamera(Mango::CameraComponent& camera, Mango::TransformComponent& transform);

		friend class SceneSerializer;
	};
}
