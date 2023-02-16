#pragma once

#include "Scene.h"
#include "../Render/Renderer.h"

#include <string>

namespace Mango
{
	class SceneManager
	{
	public:
		SceneManager(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&) = delete;
		~SceneManager();

		static void SetRenderer(Mango::Renderer* renderer);
		static void LoadFromJson(std::string& sceneJson);
		static void LoadEmpty();
		static Mango::Scene& GetScene() { return *_scene; };

	private:
		static void UnloadScene();

	private:
		static Mango::Renderer* _renderer;
		static Mango::Scene* _scene;
	};
}
