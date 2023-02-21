#include "ScriptEngine.h"

#include "../GUID.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdexcept>

class Entity
{
public:
    Mango::GUID _id;
};

typedef struct
{
    PyObject_HEAD
    Entity* objPtr;
} PyEntity;

static std::string _entityModuleName = "Entity";

static PyObject* GetId(PyEntity* self, PyObject* Py_UNUSED(ignored))
{
    uint64_t id = self->objPtr->_id;
    PyObject* pyId = PyLong_FromUnsignedLongLong(id);
    Py_IncRef(pyId);
    return pyId;
}

static PyMethodDef _entityMethods[] =
{
    {
        "__init__",
        [](PyObject* self, PyObject*) -> PyObject*
        {
            Py_IncRef(Py_None);
            return Py_None;
        },
        METH_CLASS | METH_NOARGS,
        "Initialize current entity within engine"
    },
    {
        "GetId",
        (PyCFunction)GetId,
        METH_NOARGS,
        "Get current entity's ID"
    },
    {
        "GetTransform",
        [](PyObject* self, PyObject*) -> PyObject*
        {
            Py_IncRef(Py_None);
            return Py_None;
        },
        METH_NOARGS,
        "Get transform of the current entity"
    },
    { nullptr, nullptr, 0, nullptr } // This line is required, don't remove!
};

static struct PyModuleDef _mangoEngineModuleDefinition =
{
    PyModuleDef_HEAD_INIT,
    _entityModuleName.c_str(),
    nullptr, // documentation
    -1,
    nullptr // module methods
};

static PyObject* PyEntity_New(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    PyEntity* self = (PyEntity*)type->tp_alloc(type, 0);
    if (self != nullptr)
    {
        self->objPtr = new Entity();
    }
    return (PyObject*)self;
}

static int PyEntity_Init(PyEntity* self, PyObject* args, PyObject* kwds)
{
    // Here custom init code could be
    return 0;
}

static void PyEntity_Dealloc(PyEntity* self)
{
    delete self->objPtr;
    Py_TYPE(self)->tp_free(self);
}

static PyTypeObject PyEntityType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "Entity.Entity"
};

PyMODINIT_FUNC PyInit_EntityModule(void)
{
    PyEntityType.tp_new = PyEntity_New;
    PyEntityType.tp_basicsize = sizeof(PyEntity);
    PyEntityType.tp_itemsize = 0;
    PyEntityType.tp_dealloc = (destructor)PyEntity_Dealloc;
    PyEntityType.tp_flags = Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IS_ABSTRACT;
    PyEntityType.tp_doc = PyDoc_STR("Base Entity object. All scriptable classes must inherit this.");
    PyEntityType.tp_methods = _entityMethods;
    PyEntityType.tp_init = (initproc)PyEntity_Init;

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
    PyModule_AddObject(entityModule, _entityModuleName.c_str(), (PyObject*)&PyEntityType);
    return entityModule;
}

Mango::ScriptEngine::ScriptEngine()
{
    if (PyImport_AppendInittab(_entityModuleName.c_str(), PyInit_EntityModule) < 0)
    {
        throw std::runtime_error("Unable to initialize Entity python module");
    }
    Py_Initialize();

    //PyObject* modulePath = PyUnicode_DecodeFSDefault(_entityModuleName.c_str());
    //if (modulePath == nullptr)
    //{
    //    // Handle errors
    //}
    
    //PyObject* module = PyImport_Import(modulePath);
    //Py_DecRef(modulePath);

    PyRun_SimpleString("import Entity\n\n"
        "class TestEntity(Entity.Entity):\n"
        "    def __init__(self):\n"
        "        print('init')\n"
        "        pass\n\n"
        "test = TestEntity()\n"
        "test.GetId()\n"
        "a = Entity.Entity()\n"
    );
    

    PyObject* entityModule = PyImport_ImportModule(_entityModuleName.c_str());
    if (entityModule == nullptr)
    {
        throw std::runtime_error("Unable to import Entity python module");
    }

    PyObject* moduleClasses = PyModule_GetDict(entityModule);
    // Py_DecRef(module);
    PyObject* key, * value;
    Py_ssize_t position = 0;

    while (PyDict_Next(moduleClasses, &position, &key, &value))
    {
        if (PyCallable_Check(value))
        {
            // PyObject_Is
            PyObject* object = PyObject_CallNoArgs(value);
        }
    }
    
    if (Py_FinalizeEx() < 0) {
        // Handle errors
    }
}
