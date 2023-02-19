#include "ScriptEngine.h"

#include <Python.h>

#include <stdexcept>

static std::string _mangoEngineModuleName = "MangoEngine";

static PyMethodDef _mangoEngineMethods[] =
{
    {
        "GetTransform",
        [](PyObject* self, PyObject* args) -> PyObject*
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
    _mangoEngineModuleName.c_str(),
    nullptr,
    -1,
    _mangoEngineMethods
};

PyMODINIT_FUNC PyInit_MangoEngineModule(void)
{
    return PyModule_Create(&_mangoEngineModuleDefinition);
}

Mango::ScriptEngine::ScriptEngine()
{
    if (PyImport_AppendInittab(_mangoEngineModuleName.c_str(), PyInit_MangoEngineModule) < 0)
    {
        throw std::runtime_error("Unable to initialize MangoEngine python module");
    }
    Py_Initialize();
    PyRun_SimpleString("import MangoEngine as engine\n"
        "print(engine.GetTransform())\n");
    
    if (Py_FinalizeEx() < 0) {
        // Handle errors
    }
}
