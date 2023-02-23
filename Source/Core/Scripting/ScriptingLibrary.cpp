#include "ScripingLibrary.h"

#include <iostream>
#include <stdexcept>

static std::string _engineModuleName = "MangoEngine";
static std::string _entityClassName = "Entity";
static std::string _fullClassName = _engineModuleName + "." + _entityClassName;

static PyObject* GetId(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(ignored))
{
    uint64_t id = self->objPtr->_id;
    PyObject* pyId = PyLong_FromUnsignedLongLong(id);
    Py_IncRef(pyId);
    return pyId;
}

static PyObject* GetTransform(Mango::Scripting::PyEntity* self, PyObject* Py_UNUSED(ignored))
{
    Py_IncRef(Py_None);
    return Py_None;
}

static PyMethodDef _entityMethods[] =
{
    {
        "GetId",
        (PyCFunction)GetId,
        METH_NOARGS,
        "Get current entity's ID"
    },
    {
        "GetTransform",
        (PyCFunction)GetTransform,
        METH_NOARGS,
        "Get transform of the current entity"
    },
    { nullptr, nullptr, 0, nullptr } // This line is required, don't remove!
};

static struct PyModuleDef _mangoEngineModuleDefinition =
{
    PyModuleDef_HEAD_INIT,
    _engineModuleName.c_str(),
    "Standard MangoEngine module for scripting",
    -1,
    nullptr // module methods
};

static PyObject* PyEntity_New(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Mango::Scripting::PyEntity* self = (Mango::Scripting::PyEntity*)type->tp_alloc(type, 0);
    if (self != nullptr)
    {
        self->objPtr = new Mango::Scripting::ScriptableEntity();
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
    PyModule_AddObject(entityModule, _entityClassName.c_str(), (PyObject*)&PyEntityType);
    return entityModule;
}

std::string Mango::Scripting::GetLibraryName()
{
    return _engineModuleName;
}

Mango::Scripting::ModuleInitFunc Mango::Scripting::GetModuleInitializationFunction()
{
    return PyInit_EntityModule;
}
