#include "Scene.h"

Mango::Scene::Scene(Mango::Renderer& renderer)
    : _renderer(renderer)
{
	const auto entity1 = _registry.create();
    _registry.emplace<IdComponent>(entity1);
    _registry.emplace<NameComponent>(entity1);
	_registry.emplace<TransformComponent>(entity1, glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity1, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity1, Mango::GeometryType::Triangle);

    const auto entity2 = _registry.create();
    _registry.emplace<IdComponent>(entity2);
    _registry.emplace<NameComponent>(entity2);
    _registry.emplace<TransformComponent>(entity2, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity2, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity2, Mango::GeometryType::Triangle);

    const auto entity3 = _registry.create();
    _registry.emplace<IdComponent>(entity3);
    _registry.emplace<NameComponent>(entity3);
    _registry.emplace<TransformComponent>(entity3, glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity3, Mango::GeometryType::Rectangle);
}

void Mango::Scene::OnCreate()
{
}

void Mango::Scene::OnUpdate()
{
    // Iteration over registry is performed in reverse order compared in which they was added
    auto view = _registry.view<TransformComponent, ColorComponent, GeometryComponent>();
    for (auto [entity, transform, color, geometry] : view.each())
    {
        if (geometry.GetGeometry() == Mango::GeometryType::Triangle)
        {
            _renderer.DrawTriangle(transform.GetTransform(), color.GetColor());
        }
        if (geometry.GetGeometry() == Mango::GeometryType::Rectangle)
        {
            _renderer.DrawRect(transform.GetTransform(), color.GetColor());
        }
    }
}

void Mango::Scene::AddTriangle()
{
    AddDefaultEntity(Mango::GeometryType::Triangle);
}

void Mango::Scene::AddRectangle()
{
    AddDefaultEntity(Mango::GeometryType::Rectangle);
}

void Mango::Scene::DeleteEntity(Mango::GUID entityId)
{
    auto view = _registry.view<IdComponent>();
    // TODO: Iteration over whole registry may be not needed here
    for (const auto [entity, id] : view.each())
    {
        if (id.GetId() == entityId)
        {
            _registry.destroy(entity);
            return;
        }
    }
}

void Mango::Scene::AddDefaultEntity(Mango::GeometryType geometry)
{
    const auto entity = _registry.create();
    _registry.emplace<IdComponent>(entity);
    _registry.emplace<NameComponent>(entity);
    _registry.emplace<TransformComponent>(entity, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity, geometry);
}
