#include "ScriptEngine.h"

#include "ScripingLibrary.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <stdexcept>

Mango::ScriptEngine::ScriptEngine()
{
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

            // Create PyEnities
            PyObject* entityId = PyLong_FromUnsignedLongLong((uint64_t)it->first);
            PyObject* args = PyTuple_Pack(1, entityId);
            PyObject* obj = PyObject_CallObject(value, args);
            Py_DecRef(args);
            Py_DecRef(entityId);
        }
        Py_DecRef(key);
        Py_DecRef(value);
        Py_DecRef(moduleClasses);
    }
}
