#include "Scene.h"

#include "../Infrastructure/Logging/Logging.h"

#include <filesystem>
#include <unordered_map>

Mango::Scene::Scene(Mango::Renderer& renderer)
    : _renderer(renderer)
{
    _scriptEngine = std::make_unique<Mango::ScriptEngine>();
    _collisionListener = std::make_unique<CollisionListener>(_scriptEngine.get());
    _physicsWorld.SetContactListener(_collisionListener.get());
}

Mango::Scene::~Scene()
{
    _scriptEngine = nullptr;
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
    // Render
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

    _scriptEngine->OnUpdate();
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

    _scriptEngine->OnFixedUpdate();
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
        // Some precision magic apparently, fixes checkboxes that are little bigger than actual geometry
        bodyBox.SetAsBox(scale.x * 0.99, scale.y * 0.99);
        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &bodyBox;
        fixtureDefinition.density = 1.0f;
        fixtureDefinition.friction = 0.3f;
        rigidbody.SetFixture(fixtureDefinition);
    }

    // Setup ScriptEngine
    std::filesystem::path scriptsPath = std::filesystem::current_path();
    std::unordered_map<std::string, std::filesystem::path> scripts;
    for (const auto& entry : std::filesystem::directory_iterator(scriptsPath))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        scripts[entry.path().filename().string()] = entry.path();
    }

    std::unordered_map<Mango::GUID, std::filesystem::path> entitiesToScriptsMap;
    for (auto [entity, id, script] : _registry.view<IdComponent, ScriptComponent>().each())
    {
        const auto& scriptFileName = std::string(script.GetFileName());
        if (!scripts.contains(scriptFileName))
        {
            M_ERROR("Couldn't find " + scriptFileName + " script.");
        }

        const auto& scriptFilePath = scripts[scriptFileName];
        entitiesToScriptsMap[id.GetId()] = scriptFilePath;
    }

    _scriptEngine->SetUserData(this);
    _scriptEngine->SetApplyForceEventHandler(ApplyForce);
    _scriptEngine->SetGetTransformEventHandler(GetPosition);
    _scriptEngine->SetSetTransformEventHandler(SetPosition);
    _scriptEngine->SetIsKeyPressedEventHandler(GetKeyPressed);
    _scriptEngine->SetIsMouseButtonPressedEventHandler(GetMouseButtonPressed);
    _scriptEngine->SetGetMouseCursorPositionEventHandler(GetMouseCursorPosition);
    _scriptEngine->SetGetRotationEventHandler(GetRotation);
    _scriptEngine->SetSetRotationEventHandler(SetRotation);
    _scriptEngine->SetGetScaleEventHandler(GetScale);
    _scriptEngine->SetSetScaleEventHandler(SetScale);
    _scriptEngine->SetCreateEntityEventHandler(CreateEntity);
    _scriptEngine->SetDestroyEntityEventHandler(DestroyEntity);
    _scriptEngine->SetSetRigidEntityEventHandler(SetRigid);
    _scriptEngine->SetConfigureRigidbodyEventHandler(ConfigureRigidbody);

    try
    {
        _scriptEngine->LoadScripts(entitiesToScriptsMap);
    }
    catch (std::exception ex)
    {
        M_ERROR("Unable to load scripts: " + std::string(ex.what()));
    }

    // Run OnPlay on already existing entities
    _scriptEngine->OnCreate();
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
    if (_registry.try_get<RigidbodyComponent>(entity) != nullptr)
    {
        return;
    }

    auto& id = _registry.get<IdComponent>(entity).GetId();

    b2BodyDef bodyDefinition;
    bodyDefinition.userData.pointer = reinterpret_cast<uintptr_t>(&id);
    b2Body* body = _physicsWorld.CreateBody(&bodyDefinition);
    
    auto& rigidbody = _registry.emplace<RigidbodyComponent>(entity, body);
    auto& transform = _registry.get<TransformComponent>(entity);

    auto translation = transform.GetTranslation();
    auto rotation = transform.GetRotation().z;
    auto scale = transform.GetScale();

    rigidbody.SetTransform(glm::vec2(translation.x, translation.y), glm::radians(rotation));
    b2PolygonShape bodyBox;
    // Some precision magic apparently, fixes checkboxes that are little bigger than actual geometry
    bodyBox.SetAsBox(scale.x * 0.99, scale.y * 0.99);
    b2FixtureDef fixtureDefinition;
    fixtureDefinition.shape = &bodyBox;
    fixtureDefinition.density = 1.0f;
    fixtureDefinition.friction = 0.3f;
    rigidbody.SetFixture(fixtureDefinition);
}

void Mango::Scene::AddScript(entt::entity entity)
{
    auto script = _registry.try_get<ScriptComponent>(entity);
    if (script != nullptr)
    {
        return;
    }

    _registry.emplace<ScriptComponent>(entity);
}

void Mango::Scene::DeleteEntity(entt::entity entity)
{
    _registry.destroy(entity);
}

void Mango::Scene::ApplyForce(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 force)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody == nullptr)
    {
        return;
    }
    rigidbody->ApplyForce(force);
}

glm::vec2 Mango::Scene::GetPosition(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return glm::vec2(std::numeric_limits<float>().min());
    }

    auto& transform = registry.get<TransformComponent>(entity);
    auto translation = transform.GetTranslation();
    return glm::vec2(translation.x, translation.y);
}

void Mango::Scene::SetPosition(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 transform)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto& transformComponent = registry.get<TransformComponent>(entity);
    auto zPosition = transformComponent.GetTranslation().z;
    transformComponent.SetTranslation(glm::vec3(transform, zPosition));
    
    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody != nullptr)
    {
        float angleRadians = glm::radians(transformComponent.GetRotation().z);
        rigidbody->SetTransform(transform, angleRadians);
    }
}

bool Mango::Scene::GetKeyPressed(Mango::ScriptEngine* scriptEngine, Mango::Key key)
{
    auto pressedKeys = Mango::Input::GetPressedKeys();
    return pressedKeys.contains(key);
}

bool Mango::Scene::GetMouseButtonPressed(Mango::ScriptEngine* scriptEngine, Mango::MouseButton mouseButton)
{
    auto pressedMouseButtons = Mango::Input::GetPressedMouseButtons();
    return pressedMouseButtons.contains(mouseButton);
}

glm::vec2 Mango::Scene::GetMouseCursorPosition(Mango::ScriptEngine* scriptEngine)
{
    return Mango::Input::GetMouseCursorPosition();
}

float Mango::Scene::GetRotation(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return std::numeric_limits<float>().min();
    }

    auto& transform = registry.get<TransformComponent>(entity);
    return transform.GetRotation().z;
}

void Mango::Scene::SetRotation(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, float rotation)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto& transform = registry.get<TransformComponent>(entity);
    auto r = transform.GetRotation();
    transform.SetRotation(glm::vec3(r.x, r.y, rotation));

    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody != nullptr)
    {
        rigidbody->SetTransform(rigidbody->GetPosition(), glm::radians(rotation));
    }
}

glm::vec2 Mango::Scene::GetScale(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return glm::vec2(std::numeric_limits<float>().min());
    }

    auto& transform = registry.get<TransformComponent>(entity);
    auto scale = transform.GetScale();
    return glm::vec2(scale.x, scale.y);
}

void Mango::Scene::SetScale(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, glm::vec2 scale)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto& transform = registry.get<TransformComponent>(entity);
    auto scaleZ = transform.GetScale().z;
    transform.SetScale(glm::vec3(scale, scaleZ));

    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody != nullptr)
    {
        rigidbody->DestroyFixture();

        b2PolygonShape bodyBox;
        // Some precision magic apparently, fixes hitboxes that are little bigger than actual geometry
        bodyBox.SetAsBox(scale.x * 0.99, scale.y * 0.99);

        b2FixtureDef fixtureDefinition;
        fixtureDefinition.shape = &bodyBox;
        fixtureDefinition.density = 1.0f;
        fixtureDefinition.friction = 0.3f;
        rigidbody->SetFixture(fixtureDefinition);
    }
}

Mango::GUID Mango::Scene::CreateEntity(Mango::ScriptEngine* scriptEngine)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    entt::entity entity = scene->AddDefaultEntity(Mango::GeometryType::Rectangle);
    auto& registry = scene->GetRegistry();
    return registry.get<IdComponent>(entity).GetId();
}

void Mango::Scene::DestroyEntity(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();

    entt::entity entity;
    for (auto [e, id] : registry.view<IdComponent>().each())
    {
        if (id.GetId() == entityId)
        {
            entity = e;
            break;
        }
    }

    if (!registry.valid(entity))
    {
        return;
    }

    Mango::RigidbodyComponent* rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody != nullptr)
    {
        scene->_physicsWorld.DestroyBody(rigidbody->GetBody());
    }

    registry.destroy(entity);
}

void Mango::Scene::SetRigid(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, bool isRigid)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    bool shouldAddRigidbody = rigidbody == nullptr && isRigid == true;
    bool shouldRemoveRigidbody = rigidbody != nullptr && isRigid == false;
    if (shouldAddRigidbody)
    {
        scene->AddRigidbody(entity);
    }
    if (shouldRemoveRigidbody)
    {
        registry.remove<RigidbodyComponent>(entity);
    }
}

void Mango::Scene::ConfigureRigidbody(Mango::ScriptEngine* scriptEngine, Mango::GUID entityId, float density, float friction, bool isDynamic)
{
    Mango::Scene* scene = reinterpret_cast<Mango::Scene*>(scriptEngine->GetUserData());
    auto& registry = scene->GetRegistry();
    auto entity = scene->GetEntityById(entityId);
    if (!registry.valid(entity))
    {
        return;
    }

    auto rigidbody = registry.try_get<RigidbodyComponent>(entity);
    if (rigidbody == nullptr)
    {
        return;
    }

    rigidbody->SetDensity(density);
    rigidbody->SetFriction(friction);
    rigidbody->SetDynamic(isDynamic);
}

entt::entity Mango::Scene::AddDefaultEntity(Mango::GeometryType geometry)
{
    const auto entity = _registry.create();
    _registry.emplace<IdComponent>(entity);
    _registry.emplace<NameComponent>(entity);
    _registry.emplace<TransformComponent>(entity, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    _registry.emplace<ColorComponent>(entity, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    _registry.emplace<GeometryComponent>(entity, geometry);
    return entity;
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

entt::entity Mango::Scene::GetEntityById(Mango::GUID entityId)
{
    for (auto [entity, id] : _registry.view<IdComponent>().each())
    {
        if (id.GetId() == entityId)
        {
            return entity;
        }
    }
    return entt::entity();
}

void Mango::CollisionListener::BeginContact(b2Contact* contact)
{
    b2BodyUserData userData = contact->GetFixtureA()->GetBody()->GetUserData();
    Mango::GUID* firstId = reinterpret_cast<Mango::GUID*>(userData.pointer);

    userData = contact->GetFixtureB()->GetBody()->GetUserData();
    Mango::GUID* secondId = reinterpret_cast<Mango::GUID*>(userData.pointer);

    _scriptEngine->OnCollisionBegin(*firstId, *secondId);
}

void Mango::CollisionListener::EndContact(b2Contact* contact)
{
    b2BodyUserData userData = contact->GetFixtureA()->GetBody()->GetUserData();
    Mango::GUID* firstId = reinterpret_cast<Mango::GUID*>(userData.pointer);

    userData = contact->GetFixtureB()->GetBody()->GetUserData();
    Mango::GUID* secondId = reinterpret_cast<Mango::GUID*>(userData.pointer);

    _scriptEngine->OnCollisionEnd(*firstId, *secondId);
}
