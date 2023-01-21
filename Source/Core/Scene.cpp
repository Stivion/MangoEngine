#include "Scene.h"

Mango::Scene::Scene(Mango::Renderer& renderer)
    : _renderer(renderer)
{
    const auto entity3 = _registry.create();
    _registry.emplace<IdComponent>(entity3);
    _registry.emplace<TransformComponent>(entity3, glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity3, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity3, Mango::GeometryType::Rectangle);

    const auto entity2 = _registry.create();
    _registry.emplace<IdComponent>(entity2);
    _registry.emplace<TransformComponent>(entity2, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity2, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity2, Mango::GeometryType::Triangle);

	const auto entity1 = _registry.create();
    _registry.emplace<IdComponent>(entity1);
	_registry.emplace<TransformComponent>(entity1, glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity1, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity1, Mango::GeometryType::Triangle);
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
