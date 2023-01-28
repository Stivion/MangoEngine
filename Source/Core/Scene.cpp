#include "Scene.h"

Mango::Scene::Scene(Mango::Renderer& renderer)
    : _renderer(renderer)
{
}

void Mango::Scene::OnCreate()
{
}

void Mango::Scene::OnUpdate()
{
    auto camerasView = _registry.view<CameraComponent, TransformComponent>();
    bool editorCameraFound = false;
    for (auto [entity, camera, transform] : camerasView.each())
    {
        if (camera.IsEditorCamera())
        {
            editorCameraFound = true;
            SetRendererCamera(camera, transform);
            break;
        }
    }
    if (!editorCameraFound)
    {
        for (auto [entity, camera, transform] : camerasView.each())
        {
            if (camera.IsPrimary())
            {
                SetRendererCamera(camera, transform);
                break;
            }
        }
    }

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

void Mango::Scene::AddCamera()
{
    const auto editorCamera = _registry.create();
    _registry.emplace<IdComponent>(editorCamera);
    _registry.emplace<NameComponent>(editorCamera);
    _registry.emplace<TransformComponent>(editorCamera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    _registry.emplace<CameraComponent>(editorCamera, false);
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

void Mango::Scene::SetRendererCamera(Mango::CameraComponent& camera, Mango::TransformComponent& transform)
{
    RendererCameraInfo cameraInfo{};
    cameraInfo.NearPlane = camera.GetNearPlane();
    cameraInfo.FarPlane = camera.GetFarPlane();
    cameraInfo.FovDegrees = camera.GetFOV();
    cameraInfo.Position = transform.GetTranslation();
    cameraInfo.ViewTarget = camera.GetViewTarget();
    _renderer.SetCamera(cameraInfo);
}
