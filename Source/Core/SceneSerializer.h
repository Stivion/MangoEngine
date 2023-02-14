#pragma once

#include "Scene.h"

#include <string>

namespace Mango
{
	class SceneSerializer
	{
	public:
		std::string Serialize(Mango::Scene& scene);
		Mango::Scene Deserialize(std::string& sceneJson);
	};
}
