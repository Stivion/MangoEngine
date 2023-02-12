#include "Scene.h"

Mango::Scene::Scene(Mango::Renderer& renderer)
    : _renderer(renderer)
{
}

void Mango::Scene::OnCreate()
{
    bool editorCameraFound = false;
    auto camerasView = _registry.view<CameraComponent, TransformComponent>();
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

    // Update camera views
    auto camerasView = _registry.view<CameraComponent, TransformComponent>();
    for (auto [entity, camera, transform] : camerasView.each())
    {
        if (_sceneState == Mango::SceneState::Play)
        {
            if (camera.IsPrimary())
            {
                SetRendererCamera(camera, transform);
            }
        }
        else if (_sceneState == Mango::SceneState::Stop)
        {
            if (camera.IsEditorCamera())
            {
                SetRendererCamera(camera, transform);
            }
        }
    }
}

void Mango::Scene::OnFixedUpdate()
{

}

void Mango::Scene::OnPlay()
{
    if (_sceneState == Mango::SceneState::Play)
    {
        return;
    }

    bool cameraFound = false;
    auto camerasView = _registry.view<CameraComponent, TransformComponent>();
    for (auto [entity, camera, transform] : camerasView.each())
    {
        if (camera.IsPrimary())
        {
            cameraFound = true;
            SetRendererCamera(camera, transform);
            break;
        }
    }

    if (cameraFound)
    {
        _sceneState = Mango::SceneState::Play;
    }
}

void Mango::Scene::OnStop()
{
    if (_sceneState == Mango::SceneState::Stop)
    {
        return;
    }

    _sceneState = Mango::SceneState::Stop;

    auto camerasView = _registry.view<CameraComponent, TransformComponent>();
    for (auto [entity, camera, transform] : camerasView.each())
    {
        if (camera.IsEditorCamera())
        {
            SetRendererCamera(camera, transform);
            break;
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

entt::entity Mango::Scene::AddCamera()
{
    const auto editorCamera = _registry.create();
    _registry.emplace<IdComponent>(editorCamera);
    _registry.emplace<NameComponent>(editorCamera);
    _registry.emplace<TransformComponent>(editorCamera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    _registry.emplace<CameraComponent>(editorCamera, false);
    return editorCamera;
}

void Mango::Scene::AddRigidbody(entt::entity entity)
{
    auto rigidbody = _registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody != nullptr)
    {
        return;
    }

    _registry.emplace<RigidbodyComponent>(entity);
}

void Mango::Scene::DeleteEntity(entt::entity entity)
{
    _registry.destroy(entity);
}

void Mango::Scene::AddDefaultEntity(Mango::GeometryType geometry)
{
    const auto entity = _registry.create();
    _registry.emplace<IdComponent>(entity);
    _registry.emplace<NameComponent>(entity);
    _registry.emplace<TransformComponent>(entity, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity, geometry);
}

void Mango::Scene::SetRendererCamera(Mango::CameraComponent& camera, Mango::TransformComponent& transform)
{
    RendererCameraInfo cameraInfo{};
    cameraInfo.NearPlane = camera.GetNearPlane();
    cameraInfo.FarPlane = camera.GetFarPlane();
    cameraInfo.FovDegrees = camera.GetFOV();
    cameraInfo.Translation = transform.GetTranslation();
    cameraInfo.Rotation = transform.GetRotation();
    _renderer.SetCamera(cameraInfo);
}
