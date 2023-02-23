#include "ScriptEngine.h"

#include "ScripingLibrary.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>
#include <stdexcept>

Mango::ScriptEngine::ScriptEngine()
{
    const auto engineModuleName = Mango::Scripting::GetLibraryName();

    if (PyImport_AppendInittab(engineModuleName.c_str(), Mango::Scripting::GetModuleInitializationFunction()) < 0)
    {
        throw std::runtime_error("Unable to initialize MangoEngine python module");
    }
    Py_Initialize();

    PyRun_SimpleString(
        "import MangoEngine\n"
        "\n"
        "class TestEntity(MangoEngine.Entity):"
        "\n"
        "    def __init__(self):\n"
        "        print('init')\n"
        "        pass\n"
        "\n"
        "\n"
        "test = TestEntity()\n"
        "test.GetId()\n"
        "a = MangoEngine.Entity()\n"
    );

    PyObject* engineModule = PyImport_ImportModule(engineModuleName.c_str());
    if (engineModule == nullptr)
    {
        throw std::runtime_error("Unable to import MangoEngine python module");
    }

    PyObject* moduleClasses = PyModule_GetDict(engineModule);
    Py_DecRef(engineModule);
    PyObject* key, * value;
    Py_ssize_t position = 0;

    while (PyDict_Next(moduleClasses, &position, &key, &value))
    {
        if (PyCallable_Check(value))
        {
            // PyObject_IsSubclass
            PyObject* o = PyObject_CallObject(value, nullptr);
            std::cout << o->ob_type->tp_name << '\n';
            if (PyCallable_Check(o))
            {
                PyObject_CallNoArgs(o);
            }
            // PyObject* object = PyObject_CallNoArgs(value);
        }
    }

    Py_DecRef(moduleClasses);
    
    if (Py_FinalizeEx() < 0) {
        // Handle errors
    }
}
