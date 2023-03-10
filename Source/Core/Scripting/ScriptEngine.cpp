#include "ScriptEngine.h"

#include "../../Infrastructure/Logging/Logging.h"

#include <algorithm>
#include <stdexcept>

Mango::ScriptEngine::ScriptEngine()
{
    // Initialize events
    Mango::Scripting::SetUserPointer(this);
    Mango::Scripting::SetScriptEventHanlder(HandleScriptEvent);

    // Make engine scripting library available for import from Python
    const auto engineModuleName = Mango::Scripting::GetLibraryName();
    if (PyImport_AppendInittab(engineModuleName.c_str(), Mango::Scripting::GetModuleInitializationFunction()) < 0)
    {
        PyErr_Print();
        throw std::runtime_error("Unable to initialize MangoEngine python module");
    }

    Py_Initialize();

    // Import engine scripting library from Python side
    PyObject* engineModule = PyImport_ImportModule(engineModuleName.c_str());
    if (engineModule == nullptr)
    {
        PyErr_Print();
        throw std::runtime_error("Unable to import MangoEngine python module");
    }
    // Py_DecRef(engineModule);
}

Mango::ScriptEngine::~ScriptEngine()
{
    for (auto& [_, entity] : _entities)
    {
        Py_DecRef(entity);
    }

    for (auto it = _loadedModules.begin(); it != _loadedModules.end(); it++)
    {
        Py_DecRef(it->second);
    }

    if (Py_FinalizeEx() < 0)
    {
        PyErr_Print();
    }
}

void Mango::ScriptEngine::LoadScripts(std::unordered_map<Mango::GUID, std::filesystem::path> entitiesToScriptsMap)
{
    // NOTE: Entities not freed here because Python interpreter will crash after some reloads
    _entities.clear();

    // Iterate over all scripts from engine editor
    for (auto it = entitiesToScriptsMap.begin(); it != entitiesToScriptsMap.end(); it++)
    {
        const std::string scriptName = it->second.stem().string();
        PyObject* module = nullptr;
        
        if (_loadedModules.contains(scriptName))
        {
            // NOTE: This reimport is probably leaking memory and I have no clue how to fix this. Seems like it lies deep inside Python itself.
            Py_DecRef(_loadedModules[scriptName]);
            module = PyImport_ReloadModule(_loadedModules[scriptName]);
        }
        else
        {
            module = PyImport_ImportModule(scriptName.c_str());
        }
        if (module == nullptr)
        {
            PyErr_Print();
            M_ERROR("Unable to load Python script: " + scriptName);
        }
        _loadedModules[scriptName] = module;

        // Scan module
        PyObject* moduleClasses = PyModule_GetDict(module);
        PyObject* key, * value;
        Py_ssize_t position = 0;
        // Iterate over all entries in module
        while (PyDict_Next(moduleClasses, &position, &key, &value))
        {
            // Make sure that we can call this object
            if (!PyCallable_Check(value))
            {
                continue;
            }

            // Skip all classes that don't inherit from base MangoEngine.Entity class
            if (!PyObject_IsSubclass(value, Mango::Scripting::GetEntityType()))
            {
                continue;
            }

            // Skip all functions that aren't bound by a class
            if (PyFunction_Check(value))
            {
                continue;
            }

            // Create PyEnities
            PyObject* entityId = PyLong_FromUnsignedLongLong((uint64_t)it->first);
            PyObject* args = PyTuple_Pack(1, entityId);
            PyObject* obj = PyObject_CallObject(value, args);
            _entities[it->first] = obj;
            Py_DecRef(args);
            Py_DecRef(entityId);
        }
        Py_DecRef(key);
        Py_DecRef(value);
        Py_DecRef(moduleClasses);
    }
}

void Mango::ScriptEngine::OnCreate(std::uint64_t entityId)
{
    PyObject* entity = _entities[entityId];
    CallMethod(entity, "OnCreate");
}

void Mango::ScriptEngine::OnCreate()
{
    for (auto& [_, entity] : _entities)
    {
        CallMethod(entity, "OnCreate");
    }
}

void Mango::ScriptEngine::OnUpdate()
{
    if (!_markedForDeletionEntities.empty())
    {
        for (auto& entityId : _markedForDeletionEntities)
        {
            DeletePyEntity(entityId);
        }
        _markedForDeletionEntities.clear();
    }

    for (auto& [_, entity] : _entities)
    {
        CallMethod(entity, "OnUpdate");
    }
}

void Mango::ScriptEngine::OnFixedUpdate()
{
    if (!_markedForDeletionEntities.empty())
    {
        for (auto& entityId : _markedForDeletionEntities)
        {
            DeletePyEntity(entityId);
        }
        _markedForDeletionEntities.clear();
    }

    for (auto& [_, entity] : _entities)
    {
        CallMethod(entity, "OnFixedUpdate");
    }

    CallOnCollisionBegin();
    CallOnCollisionEnd();
}

void Mango::ScriptEngine::OnCollisionBegin(Mango::GUID first, Mango::GUID second)
{
    // Entity could not exist here, because we don't create instances for entities without ScriptComponent
    // Just create new MangoEngine.Entity and pass it to method. This will enable proper lifecycle of this object
    if (!_entities.contains(first))
    {
        auto entity = PyObject_New(Mango::Scripting::PyEntity, Mango::Scripting::GetEntityTypeRaw());
        entity->objPtr = new Mango::Scripting::ScriptableEntity();
        entity->objPtr->_id = first;
        _entities[first] = (PyObject*)entity;
    }
    if (!_entities.contains(second))
    {
        auto entity = PyObject_New(Mango::Scripting::PyEntity, Mango::Scripting::GetEntityTypeRaw());
        entity->objPtr = new Mango::Scripting::ScriptableEntity();
        entity->objPtr->_id = second;
        _entities[second] = (PyObject*)entity;
    }

    _onCollisionBeginCallList.push_back(std::make_pair(first, second));
}

void Mango::ScriptEngine::OnCollisionEnd(Mango::GUID first, Mango::GUID second)
{
    // All entities should always exist in ScriptEngine at this point
    _onCollisionEndCallList.push_back(std::make_pair(first, second));
}

void Mango::ScriptEngine::CallOnCollisionBegin()
{
    for (auto& [first, second] : _onCollisionBeginCallList)
    {
        PyObject* firstEntity = _entities[first];
        PyObject* secondEntity = _entities[second];
        CallMethod(firstEntity, secondEntity, "OnCollisionBegin");
    }
    _onCollisionBeginCallList.clear();
}

void Mango::ScriptEngine::CallOnCollisionEnd()
{
    for (auto& [first, second] : _onCollisionEndCallList)
    {
        PyObject* firstEntity = _entities[first];
        PyObject* secondEntity = _entities[second];
        CallMethod(firstEntity, secondEntity, "OnCollisionEnd");
    }
    _onCollisionEndCallList.clear();
}

void Mango::ScriptEngine::CallMethod(PyObject* entity, std::string methodName)
{
    PyObject* method = PyUnicode_DecodeFSDefault(methodName.c_str());
    PyObject_CallMethodNoArgs(entity, method);
    Py_DecRef(method);
}

void Mango::ScriptEngine::CallMethod(PyObject* entity, PyObject* args, std::string methodName)
{
    PyObject* method = PyUnicode_DecodeFSDefault(methodName.c_str());
    PyObject_CallMethodOneArg(entity, method, args);
    Py_DecRef(method);
}

void Mango::ScriptEngine::DeletePyEntity(Mango::GUID entityId)
{
    PyObject* entity = _entities[entityId];
    Py_DecRef(entity);
    _entities.erase(entityId);
}

PyObject* Mango::ScriptEngine::HandleScriptEvent(Mango::Scripting::ScriptEvent event)
{
    Mango::ScriptEngine* scriptEngine = reinterpret_cast<Mango::ScriptEngine*>(Mango::Scripting::GetUserPointer());
    if (event.EventName == "ApplyForce")
    {
        return scriptEngine->HandleApplyForceEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "GetPosition")
    {
        return scriptEngine->HandleGetPositionEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "SetPosition")
    {
        return scriptEngine->HandleSetPositionEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "IsKeyPressed")
    {
        return scriptEngine->HandleIsKeyPressedEvent(event.Args);
    }
    else if (event.EventName == "IsMouseButtonPressed")
    {
        return scriptEngine->HandleIsMouseButtonPressedEvent(event.Args);
    }
    else if (event.EventName == "GetCursorPosition")
    {
        return scriptEngine->HandleGetMouseCursorPositionEvent();
    }
    else if (event.EventName == "GetRotation")
    {
        return scriptEngine->HandleGetRotationEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "SetRotation")
    {
        return scriptEngine->HandleSetRotationEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "GetScale")
    {
        return scriptEngine->HandleGetScaleEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "SetScale")
    {
        return scriptEngine->HandleSetScaleEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "CreateEntity")
    {
        return scriptEngine->HandleCreateEntityEvent();
    }
    else if (event.EventName == "DestroyEntity")
    {
        return scriptEngine->HandleDestroyEntityEvent(event.Args);
    }
    else if (event.EventName == "SetRigid")
    {
        return scriptEngine->HandleSetRigidEntityEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "ConfigureRigidbody")
    {
        return scriptEngine->HandleConfigureRigidbodyEvent(event.ScriptableEntity, event.Args);
    }
    else if (event.EventName == "FindEntityByName")
    {
        return scriptEngine->HandleFindEntityByNameEvent(event.Args);
    }
    Py_IncRef(Py_None);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleApplyForceEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyX = PyTuple_GetItem(args, 0);
    PyObject* pyY = PyTuple_GetItem(args, 1);
    float x = PyFloat_AsDouble(pyX);
    float y = PyFloat_AsDouble(pyY);
    glm::vec2 force = glm::vec2(x, y);
    _applyForceHandler(this, entityId, force);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleGetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    glm::vec2 transform = _getPositionHandler(this, entityId);
    PyObject* pyX = PyFloat_FromDouble((double)transform.x);
    PyObject* pyY = PyFloat_FromDouble((double)transform.y);
    return PyTuple_Pack(2, pyX, pyY);
}

PyObject* Mango::ScriptEngine::HandleSetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyX = PyTuple_GetItem(args, 0);
    PyObject* pyY = PyTuple_GetItem(args, 1);
    float x = PyFloat_AsDouble(pyX);
    float y = PyFloat_AsDouble(pyY);
    _setPositionHandler(this, entityId, glm::vec2(x, y));
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleIsKeyPressedEvent(PyObject* args)
{
    PyObject* pyKey = PyTuple_GetItem(args, 0);
    uint32_t keyCode = PyLong_AsUnsignedLong(pyKey);
    bool isPressed = _isKeyPressedHandler(this, static_cast<Mango::Key>(keyCode));
    return isPressed ? Py_True : Py_False;
}

PyObject* Mango::ScriptEngine::HandleIsMouseButtonPressedEvent(PyObject* args)
{
    PyObject* pyKey = PyTuple_GetItem(args, 0);
    uint32_t keyCode = PyLong_AsUnsignedLong(pyKey);
    bool isPressed = _isMouseButtonPressedHandler(this, static_cast<Mango::MouseButton>(keyCode));
    return isPressed ? Py_True : Py_False;
}

PyObject* Mango::ScriptEngine::HandleGetMouseCursorPositionEvent()
{
    glm::vec2 mousePosition = _getMouseCursorPositionEventHandler(this);
    PyObject* pyX = PyFloat_FromDouble(mousePosition.x);
    PyObject* pyY = PyFloat_FromDouble(mousePosition.y);
    return PyTuple_Pack(2, pyX, pyY);
}

PyObject* Mango::ScriptEngine::HandleGetRotationEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    float rotation = _getRotationEventHandler(this, entityId);
    return PyFloat_FromDouble(rotation);
}

PyObject* Mango::ScriptEngine::HandleSetRotationEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyRotation = PyTuple_GetItem(args, 0);
    float rotation = PyFloat_AsDouble(pyRotation);
    _setRotationEventHandler(this, entityId, rotation);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleGetScaleEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    glm::vec2 scale = _getScaleEventHandler(this, entityId);
    PyObject* pyScaleX = PyFloat_FromDouble(scale.x);
    PyObject* pyScaleY = PyFloat_FromDouble(scale.y);
    return PyTuple_Pack(2, pyScaleX, pyScaleY);
}

PyObject* Mango::ScriptEngine::HandleSetScaleEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyScaleX = PyTuple_GetItem(args, 0);
    PyObject* pyScaleY = PyTuple_GetItem(args, 1);
    float scaleX = PyFloat_AsDouble(pyScaleX);
    float scaleY = PyFloat_AsDouble(pyScaleY);
    _setScaleEventHandler(this, entityId, glm::vec2(scaleX, scaleY));
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleCreateEntityEvent()
{
    Mango::GUID entityId = _createEntityEventHandler(this);
    auto entity = PyObject_New(Mango::Scripting::PyEntity, Mango::Scripting::GetEntityTypeRaw());
    entity->objPtr = new Mango::Scripting::ScriptableEntity();
    entity->objPtr->_id = entityId;
    return (PyObject*)entity;
}

PyObject* Mango::ScriptEngine::HandleDestroyEntityEvent(PyObject* args)
{
    PyObject* entity = PyTuple_GetItem(args, 0);
    if (!PyObject_IsInstance(entity, Mango::Scripting::GetEntityType()))
    {
        return Py_None;
    }

    auto scriptableEntity = (Mango::Scripting::PyEntity*)entity;
    Mango::GUID entityId(scriptableEntity->objPtr->_id);
    if (std::find(_markedForDeletionEntities.begin(), _markedForDeletionEntities.end(), entityId) != _markedForDeletionEntities.end())
    {
        return Py_None;
    }

    _destroyEntityEventHandler(this, entityId);
    _markedForDeletionEntities.push_back(entityId);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleSetRigidEntityEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyIsRigid = PyTuple_GetItem(args, 0);
    bool isRigid = Py_IsTrue(pyIsRigid);
    _setRigidEntityEventHandler(this, entityId, isRigid);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleConfigureRigidbodyEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args)
{
    Mango::GUID entityId(entity->_id);
    PyObject* pyDensity = PyTuple_GetItem(args, 0);
    PyObject* pyFriction = PyTuple_GetItem(args, 1);
    PyObject* pyDynamic = PyTuple_GetItem(args, 2);
    float density = PyFloat_AsDouble(pyDensity);
    float friction = PyFloat_AsDouble(pyFriction);
    bool dynamic = Py_IsTrue(pyDynamic);
    _configureRigidbodyEventHandler(this, entityId, density, friction, dynamic);
    return Py_None;
}

PyObject* Mango::ScriptEngine::HandleFindEntityByNameEvent(PyObject* args)
{
    PyObject* pyEntityName = PyTuple_GetItem(args, 0);
    std::string entityName = PyUnicode_AsUTF8(pyEntityName);
    auto entityId = _findEntityByNameEventHandler(this, entityName);
    if (entityId == Mango::GUID::Empty())
    {
        return Py_None;
    }

    if (!_entities.contains(entityId))
    {
        auto entity = PyObject_New(Mango::Scripting::PyEntity, Mango::Scripting::GetEntityTypeRaw());
        entity->objPtr = new Mango::Scripting::ScriptableEntity();
        entity->objPtr->_id = entityId;
        _entities[entityId] = (PyObject*)entity;
    }

    return _entities[entityId];
}
