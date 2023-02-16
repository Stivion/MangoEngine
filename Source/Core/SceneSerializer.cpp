#include "SceneSerializer.h"

#include "Components/Components.h"
#include "GUID.h"

#include <glm/glm.hpp>

#include <vector>
#include <stdexcept>

std::string Mango::SceneSerializer::Serialize(Mango::Scene& scene)
{
	nlohmann::json json;

	const auto count = scene._registry.size();
	const entt::entity* entity = scene._registry.data();

	json["entities"] = nlohmann::json::array();

	for (auto i = 0; i < count; i++, entity++)
	{
		auto currentEntity = nlohmann::json::object();
		currentEntity["components"] = nlohmann::json::object();

		// IdComponent
		const auto id = scene._registry.try_get<IdComponent>(*entity);
		if (id != nullptr)
		{
			currentEntity["components"]["idComponent"] = nlohmann::json::object({ {"id", static_cast<uint64_t>(id->GetId())} });
		}

		// NameComponent
		const auto name = scene._registry.try_get<NameComponent>(*entity);
		if (name != nullptr)
		{
			currentEntity["components"]["nameComponent"] = nlohmann::json::object({ { "name", std::string(name->GetName()) } });
		}

		// TransformComponent
		const auto transform = scene._registry.try_get<TransformComponent>(*entity);
		if (transform != nullptr)
		{
			const auto translation = transform->GetTranslation();
			const auto rotation = transform->GetRotation();
			const auto scale = transform->GetScale();

			currentEntity["components"]["transformComponent"] = nlohmann::json::object({
				{ "translation", nlohmann::json::array({ translation.x, translation.y, translation.z }) },
				{ "rotation", nlohmann::json::array({ rotation.x, rotation.y, rotation.z })},
				{ "scale", nlohmann::json::array({ scale.x, scale.y, scale.z }) }
			});
		}

		// ColorComponent
		const auto color = scene._registry.try_get<ColorComponent>(*entity);
		if (color != nullptr)
		{
			const auto colorVec = color->GetColor();
			currentEntity["components"]["colorComponent"] = nlohmann::json::object({
				{ "color", nlohmann::json::array({ colorVec.r, colorVec.g, colorVec.b, colorVec.a }) }
			});
		}

		// GeometryComponent
		const auto geometry = scene._registry.try_get<GeometryComponent>(*entity);
		if (geometry != nullptr)
		{
			currentEntity["components"]["geometryComponent"] = nlohmann::json::object({ { "geometry", geometry->GetGeometry() } });
		}

		// CameraComponent
		const auto camera = scene._registry.try_get<CameraComponent>(*entity);
		if (camera != nullptr)
		{
			currentEntity["components"]["cameraComponent"] = nlohmann::json::object({
				{ "nearPlane", camera->GetNearPlane() },
				{ "farPlane", camera->GetFarPlane() },
				{ "fovDegrees", camera->GetFOV() },
				{ "isPrimary", camera->IsPrimary() },
				{ "isEditorCamera", camera->IsEditorCamera() }
			});
		}

		// RigidbodyComponent
		const auto rigidbody = scene._registry.try_get<RigidbodyComponent>(*entity);
		if (rigidbody != nullptr)
		{
			currentEntity["components"]["rigidbodyComponent"] = nlohmann::json::object({
				{ "isDynamic", rigidbody->IsDynamic() }
			});
		}

		json["entities"].push_back(currentEntity);
	}

	return json.dump(4);
}

void Mango::SceneSerializer::Populate(Mango::Scene& scene, std::string& sceneJson)
{
	auto json = nlohmann::json::parse(sceneJson);
	const auto& entities = json["entities"];
	auto& registry = scene._registry;

	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		entt::entity entity = registry.create();

		const auto& currentComponents = (*it)["components"];

		// IdComponent
		EnsureComponentExists(currentComponents, "idComponent");
		uint64_t idValue = currentComponents["idComponent"]["id"];
		Mango::GUID id(idValue);
		registry.emplace<IdComponent>(entity, id);

		// NameComponent
		EnsureComponentExists(currentComponents, "nameComponent");
		std::string name = currentComponents["nameComponent"]["name"];
		registry.emplace<NameComponent>(entity, name);
		
		// TransformComponent
		EnsureComponentExists(currentComponents, "transformComponent");
		const auto& transformJson = currentComponents["transformComponent"];
		glm::vec3 translation = glm::vec3(transformJson["translation"][0], transformJson["translation"][1], transformJson["translation"][2]);
		glm::vec3 rotation = glm::vec3(transformJson["rotation"][0], transformJson["rotation"][1], transformJson["rotation"][2]);
		glm::vec3 scale = glm::vec3(transformJson["scale"][0], transformJson["scale"][1], transformJson["scale"][2]);
		registry.emplace<TransformComponent>(entity, translation, rotation, scale);

		// ColorComponent
		if (currentComponents.contains("colorComponent"))
		{
			const auto& colorJson = currentComponents["colorComponent"];
			glm::vec4 color = glm::vec4(colorJson["color"][0], colorJson["color"][1], colorJson["color"][2], colorJson["color"][3]);
			registry.emplace<ColorComponent>(entity, color);
		}
		
		// GeometryComponent
		if (currentComponents.contains("geometryComponent"))
		{
			Mango::GeometryType geometry = currentComponents["geometryComponent"]["geometry"];
			registry.emplace<GeometryComponent>(entity, geometry);
		}
		
		// CameraComponent
		if (currentComponents.contains("cameraComponent"))
		{
			const auto& cameraJson = currentComponents["cameraComponent"];
			float nearPlane = cameraJson["nearPlane"];
			float farPlane = cameraJson["farPlane"];
			float fovDegrees = cameraJson["fovDegrees"];
			bool isPrimary = cameraJson["isPrimary"];
			bool isEditorCamera = cameraJson["isEditorCamera"];
			auto& camera = registry.emplace<CameraComponent>(entity, isEditorCamera);
			camera.SetClippingPlanes(nearPlane, farPlane);
			camera.SetFOV(fovDegrees);
			camera.SetPrimary(isPrimary);
		}

		// RigidbodyComponent
		if (currentComponents.contains("rigidbodyComponent"))
		{
			const auto& rigidbodyJson = currentComponents["rigidbodyComponent"];
			bool isDynamic = rigidbodyJson["isDynamic"];
			b2BodyDef bodyDefinition;
			b2Body* body = scene._physicsWorld.CreateBody(&bodyDefinition);
			auto& component = registry.emplace<RigidbodyComponent>(entity, body);
			component.SetDynamic(isDynamic);
		}
	}
}

void Mango::SceneSerializer::EnsureComponentExists(nlohmann::json json, const std::string& componentName)
{
	if (!json.contains(componentName))
	{
		throw std::runtime_error("Unable to deserialize " + componentName);
	}
}
