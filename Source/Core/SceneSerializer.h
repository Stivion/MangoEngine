#pragma once

#include "Scene.h"

#include <nlohmann/json.hpp>

#include <string>

namespace Mango
{
	class SceneSerializer
	{
	public:
		std::string Serialize(Mango::Scene& scene);
		void Populate(Mango::Scene& scene, std::string& sceneJson);

	private:
		void EnsureComponentExists(nlohmann::json json, const std::string& componentName);
	};
}
