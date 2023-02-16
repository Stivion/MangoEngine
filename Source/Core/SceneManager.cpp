#include "SceneManager.h"

#include "SceneSerializer.h"

Mango::Renderer* Mango::SceneManager::_renderer = nullptr;
Mango::Scene* Mango::SceneManager::_scene = nullptr;

Mango::SceneManager::~SceneManager()
{
	UnloadScene();
}

void Mango::SceneManager::SetRenderer(Mango::Renderer* renderer)
{
	_renderer = renderer;
}

void Mango::SceneManager::LoadFromJson(std::string& sceneJson)
{
	UnloadScene();
	Mango::SceneSerializer serializer;
	_scene = new Mango::Scene(*_renderer);
	serializer.Populate(*_scene, sceneJson);
}

void Mango::SceneManager::LoadEmpty()
{
	UnloadScene();
	_scene = new Mango::Scene(*_renderer);
}

void Mango::SceneManager::UnloadScene()
{
	if (_scene != nullptr)
	{
		delete _scene;
		_scene = nullptr;
	}
}
