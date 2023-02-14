#include "SceneSerializer.h"

#include "Components/Components.h"

#include <nlohmann/json.hpp>

#include <vector>

std::string Mango::SceneSerializer::Serialize(Mango::Scene& scene)
{
	nlohmann::json j;

	const auto count = scene._registry.size();
	const entt::entity* entity = scene._registry.data();

	j["entities"] = nlohmann::json::array();

	for (auto i = 0; i < count; i++, entity++)
	{
		auto currentEntity = nlohmann::json::object();
		currentEntity["components"] = nlohmann::json::array();

		// IdComponent
		const auto id = scene._registry.try_get<IdComponent>(*entity);
		if (id != nullptr)
		{
			currentEntity["components"].push_back({ { "idComponent", nlohmann::json::object({ { "id", static_cast<uint64_t>(id->GetId())} }) } });
		}

		// NameComponent
		const auto name = scene._registry.try_get<NameComponent>(*entity);
		if (name != nullptr)
		{
			currentEntity["components"].push_back({ { "nameComponent", nlohmann::json::object({ { "name", std::string(name->GetName()) } }) } });
		}

		// TransformComponent
		const auto transform = scene._registry.try_get<TransformComponent>(*entity);
		if (transform != nullptr)
		{
			const auto translation = transform->GetTranslation();
			const auto rotation = transform->GetRotation();
			const auto scale = transform->GetScale();

			const auto transformJson = nlohmann::json::object({
				{ "translation", nlohmann::json::array({ translation.x, translation.y, translation.z }) },
				{ "rotation", nlohmann::json::array({ rotation.x, rotation.y, rotation.z })},
				{ "scale", nlohmann::json::array({ scale.x, scale.y, scale.z }) }
			});
			currentEntity["components"].push_back({ { "transformComponent", transformJson} });
		}

		// ColorComponent
		const auto color = scene._registry.try_get<ColorComponent>(*entity);
		if (color != nullptr)
		{
			const auto colorVec = color->GetColor();
			const auto colorJson = nlohmann::json::object({
				{ "color", nlohmann::json::array({ colorVec.r, colorVec.g, colorVec.b, colorVec.a }) }
			});
			currentEntity["components"].push_back({ { "colorComponent", colorJson } });
		}

		// GeometryComponent
		const auto geometry = scene._registry.try_get<GeometryComponent>(*entity);
		if (geometry != nullptr)
		{
			currentEntity["components"].push_back({ { "geometryComponent", nlohmann::json::object({ "geometry", geometry->GetGeometry() }) } });
		}

		// CameraComponent
		const auto camera = scene._registry.try_get<CameraComponent>(*entity);
		if (camera != nullptr)
		{
			const auto cameraJson = nlohmann::json::object({
				{ "nearPlane", camera->GetNearPlane() },
				{ "farPlane", camera->GetFarPlane() },
				{ "fovDegrees", camera->GetFOV() },
				{ "isPrimary", camera->IsPrimary() },
				{ "isEditorCamera", camera->IsEditorCamera() }
			});
			currentEntity["components"].push_back({ { "cameraComponent", cameraJson } });
		}

		// RigidbodyComponent
		const auto rigidbody = scene._registry.try_get<RigidbodyComponent>(*entity);
		if (rigidbody != nullptr)
		{
			// TODO: map fields after refactoring RigidbodyComponent
			const auto rigidbodyJson = nlohmann::json::object({
				
			});
			currentEntity["components"].push_back({ { "rigidbodyComponent", rigidbodyJson } });
		}

		j["entities"].push_back(currentEntity);
	}

	return j.dump(4);
}

Mango::Scene Mango::SceneSerializer::Deserialize(std::string& sceneJson)
{
	
}
