#pragma once

#include "ScripingLibrary.h"
#include "../Input.h"
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
		typedef glm::vec2 (*GetPositionEventHandler)(Mango::ScriptEngine*, Mango::GUID);
		typedef void (*SetPositionEventHandler)(Mango::ScriptEngine*, Mango::GUID, glm::vec2);
		typedef bool (*IsKeyPressedEventHandler)(Mango::ScriptEngine*, Mango::Key);
		typedef bool (*IsMouseButtonPressedEventHandler)(Mango::ScriptEngine*, Mango::MouseButton);
		typedef glm::vec2 (*GetMouseCursorPositionEventHandler)(Mango::ScriptEngine*);

		ScriptEngine();
		~ScriptEngine();

		void LoadScripts(std::unordered_map<Mango::GUID, std::filesystem::path> entitiesToScriptsMap);

		void OnCreate(std::uint64_t entityId);
		void OnCreate();
		void OnUpdate();
		void OnFixedUpdate();

	public:
		void SetApplyForceEventHandler(ApplyForceEventHandler handler) { _applyForceHandler = handler; }
		void SetGetTransformEventHandler(GetPositionEventHandler handler) { _getPositionHandler = handler; }
		void SetSetTransformEventHandler(SetPositionEventHandler handler) { _setPositionHandler = handler; }
		void SetIsKeyPressedEventHandler(IsKeyPressedEventHandler handler) { _isKeyPressedHandler = handler; }
		void SetIsMouseButtonPressedEventHandler(IsMouseButtonPressedEventHandler handler) { _isMouseButtonPressedHandler = handler; }
		void SetGetMouseCursorPositionEventHandler(GetMouseCursorPositionEventHandler handler) { _getMouseCursorPositionEventHandler = handler; }
		void SetUserData(void* data) { _userData = data; }
		void* GetUserData() { return _userData; }

	private:
		std::unordered_map<std::string, PyObject*> _loadedModules;
		std::unordered_map<std::uint64_t, PyObject*> _entities;

		void CallMethod(PyObject* entity, std::string methodName);
		static PyObject* HandleScriptEvent(Mango::Scripting::ScriptEvent event);

		PyObject* HandleApplyForceEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleGetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleSetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleIsKeyPressedEvent(PyObject* args);
		PyObject* HandleIsMouseButtonPressedEvent(PyObject* args);
		PyObject* HandleGetMouseCursorPositionEvent();

	private:
		ApplyForceEventHandler _applyForceHandler;
		GetPositionEventHandler _getPositionHandler;
		SetPositionEventHandler _setPositionHandler;
		IsKeyPressedEventHandler _isKeyPressedHandler;
		IsMouseButtonPressedEventHandler _isMouseButtonPressedHandler;
		GetMouseCursorPositionEventHandler _getMouseCursorPositionEventHandler;
		void* _userData;
	};
}
