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
    if (_sceneState != Mango::SceneState::Play)
    {
        return;
    }

    _physicsWorld.Step(_timeStep, _velocityIterations, _positionIterations);
    
    auto view = _registry.view<TransformComponent, RigidbodyComponent>();
    for (auto [entity, transform, rigidbody] : view.each())
    {
        auto position = rigidbody.GetPosition();
        float angle = rigidbody.GetAngle();
        auto currentTranslation = transform.GetTranslation();
        transform.SetTranslation(glm::vec3(position, currentTranslation.z));
        auto currentRotation = transform.GetRotation();
        transform.SetRotation(glm::vec3(currentRotation.x, currentRotation.y, glm::degrees(angle)));
    }
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

    // Setup rigidbodies
    for (auto [_, transform, rigidbody] : _registry.view<TransformComponent, RigidbodyComponent>().each())
    {
        auto translation = transform.GetTranslation();
        auto rotation = transform.GetRotation().z;
        auto scale = transform.GetScale();

        rigidbody.SetTransform(glm::vec2(translation.x, translation.y), glm::radians(rotation));
        b2PolygonShape bodyBox;
        bodyBox.SetAsBox(scale.x, scale.y);
        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &bodyBox;
        fixtureDefinition.density = 1.0f;
        fixtureDefinition.friction = 0.3f;
        rigidbody.SetFixture(fixtureDefinition);
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

    // Dispose rigidbodies
    for (auto [_, rigidbody] : _registry.view<RigidbodyComponent>().each())
    {
        rigidbody.DestroyFixture();
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

    b2BodyDef bodyDefinition;
    b2Body* body = _physicsWorld.CreateBody(&bodyDefinition);
    _registry.emplace<RigidbodyComponent>(entity, body);
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
