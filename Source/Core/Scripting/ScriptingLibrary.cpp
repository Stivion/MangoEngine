#include "ScripingLibrary.h"

#include <unordered_map>
#include <stdexcept>

static std::string _engineModuleName = "MangoEngine";
static std::string _entityClassName = "Entity";
static std::string _fullClassName = _engineModuleName + "." + _entityClassName;
static Mango::Scripting::ScriptEventHandler _eventHandler = nullptr;
static void* _userPointer = nullptr;
static std::unordered_map<std::string, int32_t> _keysMapping
{
    { "W", 1 },
    { "A", 2 },
    { "S", 3 },
    { "D", 4 },
    { "Space", 5 },
    { "ArrowUp", 6 },
    { "ArrowDown", 7 },
    { "ArrowLeft", 8 },
    { "ArrowRight", 9 },
    { "Q", 10 },
    { "E", 11 },
    { "R", 12 }
};

static std::unordered_map<std::string, int32_t> _mouseButtonsMapping
{
    { "Left", 1 },
    { "Right", 2 }
};

static PyObject* ReturnNone()
{
    Py_IncRef(Py_None);
    return Py_None;
}

// This methods is provided in order to always have something to call without need to check for method existance
static PyObject* OnCreate(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(args)) { return ReturnNone(); }
static PyObject* OnUpdate(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(args)) { return ReturnNone(); }
static PyObject* OnFixedUpdate(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(args)) { return ReturnNone(); }

static PyObject* GetId(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(args))
{
    uint64_t id = self->objPtr->_id;
    PyObject* pyId = PyLong_FromUnsignedLongLong(id);
    Py_IncRef(pyId);
    return pyId;
}

static PyObject* GetPosition(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(args))
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "GetPosition";
    event.ScriptableEntity = self->objPtr;
    event.Args = nullptr;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* SetPosition(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "SetPosition";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* ApplyForce(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "ApplyForce";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* GetRotation(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "GetRotation";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* SetRotation(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "SetRotation";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* GetScale(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "GetScale";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* SetScale(Mango::Scripting::PyEntity* self, PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "SetScale";
    event.ScriptableEntity = self->objPtr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyMethodDef _entityMethods[] =
{
    {
        "OnCreate",
        (PyCFunction)OnCreate,
        METH_NOARGS,
        "Method gets executed once when the entity is created in game world. \
         Method signature is: def OnCreate(self) -> None \
         It is a base method on MangoEngine.Entity. It could be defined on custom entities and will be called by engine."
    },
    {
        "OnUpdate",
        (PyCFunction)OnUpdate,
        METH_NOARGS,
        "Method gets executed every frame. \
         Method signature is: def OnUpdate(self) -> None \
         It is a base method on MangoEngine.Entity. It could be defined on custom entities and will be called by engine."
    },
    {
        "OnFixedUpdate",
        (PyCFunction)OnFixedUpdate,
        METH_NOARGS,
        "Method gets executed every tick physics gets updated. \
         Method signature is: def OnFixedUpdate(self) -> None \
         It is a base method on MangoEngine.Entity. It could be defined on custom entities and will be called by engine."
    },
    {
        "GetId",
        (PyCFunction)GetId,
        METH_NOARGS,
        "Get current entity's ID. \
         Call example: super().GetId() -> int"
    },
    {
        "GetPosition",
        (PyCFunction)GetPosition,
        METH_NOARGS,
        "Get position of the current entity. \
         Call example: super().GetPosition() -> (x: float, y: float)"
    },
    {
        "SetPosition",
        (PyCFunction)SetPosition,
        METH_VARARGS,
        "Set position of the current entity. \
         Call example: super().SetPosition(x: float, y: float) -> None"
    },
    {
        "GetRotation",
        (PyCFunction)GetRotation,
        METH_NOARGS,
        "Get rotation of the current entity in degress. \
         Call example: super().GetRotation() -> float"
    },
    {
        "SetRotation",
        (PyCFunction)SetRotation,
        METH_VARARGS,
        "Set rotation of the current entity in degrees. \
         Call example: super().SetRotation(angleDegress: float) -> None"
    },
    {
        "GetScale",
        (PyCFunction)GetScale,
        METH_VARARGS,
        "Get scale of the current entity. \
         Call example: super().GetScale() -> (x: float, y: float)"
    },
    {
        "SetScale",
        (PyCFunction)SetScale,
        METH_VARARGS,
        "Set scale of the current entity. \
         Call example: super().SetScale(x: float, y: float) -> None"
    },
    {
        "ApplyForce",
        (PyCFunction)ApplyForce,
        METH_VARARGS,
        "Apply force to current entity. \
         Call example: super().ApplyForce(x: float, y: float) -> None"
    },
    { nullptr, nullptr, 0, nullptr } // This line is required, don't remove!
};

static PyObject* IsKeyPressed(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "IsKeyPressed";
    event.ScriptableEntity = nullptr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* Keys(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    PyObject* pyKey = PyTuple_GetItem(args, 0);
    std::string keyName = PyUnicode_AsUTF8(pyKey);
    if (_keysMapping.contains(keyName))
    {
        PyObject* keyInt = PyLong_FromLong(_keysMapping[keyName]);
        Py_IncRef(keyInt);
        return keyInt;
    }

    PyObject* noneKey = PyLong_FromLong(0);
    Py_IncRef(noneKey);
    return noneKey;
}

static PyObject* IsMouseButtonPressed(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "IsMouseButtonPressed";
    event.ScriptableEntity = nullptr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* MouseButtons(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    PyObject* pyMouseButton = PyTuple_GetItem(args, 0);
    std::string buttonName = PyUnicode_AsUTF8(pyMouseButton);
    if (_mouseButtonsMapping.contains(buttonName))
    {
        PyObject* buttonInt = PyLong_FromLong(_mouseButtonsMapping[buttonName]);
        Py_IncRef(buttonInt);
        return buttonInt;
    }

    PyObject* noneButton = PyLong_FromLong(0);
    Py_IncRef(noneButton);
    return noneButton;
}

static PyObject* GetCursorPosition(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "GetCursorPosition";
    event.ScriptableEntity = nullptr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* CreateEntity(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "CreateEntity";
    event.ScriptableEntity = nullptr;
    event.Args = nullptr;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyObject* DestroyEntity(Mango::Scripting::PyEntity* Py_UNUSED(self), PyObject* args)
{
    Mango::Scripting::ScriptEvent event;
    event.EventName = "DestroyEntity";
    event.ScriptableEntity = nullptr;
    event.Args = args;
    PyObject* result = _eventHandler(event);
    Py_IncRef(result);
    return result;
}

static PyMethodDef _moduleMethods[]
{
    {
        "IsKeyPressed",
        (PyCFunction)IsKeyPressed,
        METH_VARARGS,
        "Is provided key is pressed. \
         Call example: MangoEngine.IsKeyPressed(key: int) -> Boolean"
    },
    {
        "Keys",
        (PyCFunction)Keys,
        METH_VARARGS,
        "Get key for specified key string. \
         Available arguments: [ W, A, S, D, Space, ArrowUp, ArrowDown, ArrowLeft, ArrowRight, Q, E, R ] \
         If provided key doesn't exist method will return not existing key. \
         Call example: MangoEngine.Keys(keyName: str) -> int"
    },
    {
        "IsMouseButtonPressed",
        (PyCFunction)IsMouseButtonPressed,
        METH_VARARGS,
        "Is provided mouse button pressed \
         Call example: MangoEngine.IsMouseButtonPressed(mouseButton: int) -> Boolean"
    },
    {
        "MouseButtons",
        (PyCFunction)MouseButtons,
        METH_VARARGS,
        "Get key for specified mouse button string. \
         Available arguments: [ Left, Right ] \
         If provided key doesn't exist method will return not existing key. \
         Call example: MangoEngine.MouseButtons(mouseButtonName: str) -> int"
    },
    {
        "GetCursorPosition",
        (PyCFunction)GetCursorPosition,
        METH_NOARGS,
        "Get current mouse cursor position. \
         Call example: MangoEngine.GetCursorPosition() -> (x: float, y: float)"
    },
    {
        "CreateEntity",
        (PyCFunction)CreateEntity,
        METH_NOARGS,
        "Create new empty entity and return it. \
         Call example: MangoEngine.CreateEntity() -> MangoEngine.Entity"
    },
    {
        "DestroyEntity",
        (PyCFunction)DestroyEntity,
        METH_VARARGS,
        "Destroy passed entity from scene. \
         Call example: MangoEngine.DestroyEntity(MangoEngine.Entity) -> None"
    },
    { nullptr, nullptr, 0, nullptr } // This line is required, don't remove!
};

static struct PyModuleDef _mangoEngineModuleDefinition =
{
    PyModuleDef_HEAD_INIT,
    _engineModuleName.c_str(),
    "Standard MangoEngine module for scripting",
    -1,
    _moduleMethods
};

static PyObject* PyEntity_New(PyTypeObject* type, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    Mango::Scripting::PyEntity* self = (Mango::Scripting::PyEntity*)type->tp_alloc(type, 0);
    if (self != nullptr)
    {
        self->objPtr = new Mango::Scripting::ScriptableEntity();
        if (PyTuple_Size(args) == 1)
        {
            PyObject* entityId = PyTuple_GetItem(args, 0);
            self->objPtr->_id = PyLong_AsUnsignedLongLong(entityId);
        }
    }
    return (PyObject*)self;
}

static int PyEntity_Init(Mango::Scripting::PyEntity* self, PyObject* args, PyObject* kwds)
{
    // Here custom init code could be
    return 0;
}

static void PyEntity_Dealloc(Mango::Scripting::PyEntity* self)
{
    delete self->objPtr;
    Py_TYPE(self)->tp_free(self);
}

static PyTypeObject PyEntityType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    _fullClassName.c_str()
};

PyMODINIT_FUNC PyInit_EntityModule(void)
{
    PyEntityType.tp_new = PyEntity_New;
    PyEntityType.tp_init = (initproc)PyEntity_Init;
    PyEntityType.tp_dealloc = (destructor)PyEntity_Dealloc;
    PyEntityType.tp_basicsize = sizeof(Mango::Scripting::PyEntity);
    PyEntityType.tp_itemsize = 0;
    PyEntityType.tp_flags = Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IS_ABSTRACT;
    PyEntityType.tp_doc = PyDoc_STR("Base Entity object. All scriptable classes must inherit this.");
    PyEntityType.tp_methods = _entityMethods;

    if (PyType_Ready(&PyEntityType) < 0)
    {
        return nullptr;
    }

    PyObject* entityModule = PyModule_Create(&_mangoEngineModuleDefinition);
    if (entityModule == nullptr)
    {
        return nullptr;
    }

    Py_IncRef((PyObject*)&PyEntityType);
    if (PyModule_AddObject(entityModule, _entityClassName.c_str(), (PyObject*)&PyEntityType) != 0)
    {
        Py_DecRef(entityModule);
        Py_DecRef((PyObject*)&PyEntityType);
        return nullptr;
    }
    return entityModule;
}

PyTypeObject* Mango::Scripting::GetEntityTypeRaw()
{
    return &PyEntityType;
}

PyObject* Mango::Scripting::GetEntityType()
{
    return (PyObject*)&PyEntityType;
}

std::string Mango::Scripting::GetLibraryName()
{
    return _engineModuleName;
}

Mango::Scripting::ModuleInitFunc Mango::Scripting::GetModuleInitializationFunction()
{
    return PyInit_EntityModule;
}

void Mango::Scripting::SetScriptEventHanlder(ScriptEventHandler handler)
{
    _eventHandler = handler;
}

void Mango::Scripting::SetUserPointer(void* pointer)
{
    _userPointer = pointer;
}

void* Mango::Scripting::GetUserPointer()
{
    return _userPointer;
}
