#pragma once

#include "ScripingLibrary.h"
#include "../GUID.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "glm/glm.hpp"

#include <string>
#include <unordered_map>
#include <filesystem>

namespace Mango
{
	class ScriptEngine
	{
	public:
		typedef void (*ApplyForceEventHandler)(Mango::ScriptEngine*, Mango::GUID, glm::vec2);

		ScriptEngine();
		~ScriptEngine();

		void LoadScripts(std::unordered_map<Mango::GUID, std::filesystem::path> entitiesToScriptsMap);

		void OnCreate(std::uint64_t entityId);
		void OnCreate();
		void OnUpdate();
		void OnFixedUpdate();

	public:
		void SetApplyForceEventHandler(ApplyForceEventHandler handler) { _applyForceHandler = handler; }
		void SetUserData(void* data) { _userData = data; }
		void* GetUserData() { return _userData; }

	private:
		std::unordered_map<std::string, PyObject*> _loadedModules;
		std::unordered_map<std::uint64_t, PyObject*> _entities;

		void CallMethod(PyObject* entity, std::string methodName);
		static PyObject* HandleScriptEvent(Mango::Scripting::ScriptEvent event);

		PyObject* HandleApplyForceEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);

	private:
		ApplyForceEventHandler _applyForceHandler;
		void* _userData;
	};
}
