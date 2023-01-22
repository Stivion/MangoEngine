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

	private:
		Mango::Renderer& _renderer;
		entt::registry _registry;
	};
}
