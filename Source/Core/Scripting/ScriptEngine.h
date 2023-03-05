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
#include <set>

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
		typedef float (*GetRotationEventHandler)(Mango::ScriptEngine*, Mango::GUID);
		typedef void (*SetRotationEventHandler)(Mango::ScriptEngine*, Mango::GUID, float);
		typedef glm::vec2 (*GetScaleEventHandler)(Mango::ScriptEngine*, Mango::GUID);
		typedef void (*SetScaleEventHandler)(Mango::ScriptEngine*, Mango::GUID, glm::vec2);
		typedef Mango::GUID (*CreateEntityEventHandler)(Mango::ScriptEngine*);
		typedef void (*DestroyEntityEventHandler)(Mango::ScriptEngine*, Mango::GUID);

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
		void SetGetRotationEventHandler(GetRotationEventHandler handler) { _getRotationEventHandler = handler; }
		void SetSetRotationEventHandler(SetRotationEventHandler handler) { _setRotationEventHandler = handler; }
		void SetGetScaleEventHandler(GetScaleEventHandler handler) { _getScaleEventHandler = handler; }
		void SetSetScaleEventHandler(SetScaleEventHandler handler) { _setScaleEventHandler = handler; }
		void SetCreateEntityEventHandler(CreateEntityEventHandler handler) { _createEntityEventHandler = handler; }
		void SetDestroyEntityEventHandler(DestroyEntityEventHandler handler) { _destroyEntityEventHandler = handler; }
		void SetUserData(void* data) { _userData = data; }
		void* GetUserData() { return _userData; }

	private:
		std::unordered_map<std::string, PyObject*> _loadedModules;
		std::unordered_map<std::uint64_t, PyObject*> _entities;
		std::set<Mango::GUID> _markedForDeletionEntities;

		void CallMethod(PyObject* entity, std::string methodName);
		void DeletePyEntity(Mango::GUID entityId);
		static PyObject* HandleScriptEvent(Mango::Scripting::ScriptEvent event);

		PyObject* HandleApplyForceEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleGetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleSetPositionEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleIsKeyPressedEvent(PyObject* args);
		PyObject* HandleIsMouseButtonPressedEvent(PyObject* args);
		PyObject* HandleGetMouseCursorPositionEvent();
		PyObject* HandleGetRotationEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleSetRotationEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleGetScaleEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleSetScaleEvent(Mango::Scripting::ScriptableEntity* entity, PyObject* args);
		PyObject* HandleCreateEntityEvent();
		PyObject* HandleDestroyEntityEvent(PyObject* args);

	private:
		ApplyForceEventHandler _applyForceHandler;
		GetPositionEventHandler _getPositionHandler;
		SetPositionEventHandler _setPositionHandler;
		IsKeyPressedEventHandler _isKeyPressedHandler;
		IsMouseButtonPressedEventHandler _isMouseButtonPressedHandler;
		GetMouseCursorPositionEventHandler _getMouseCursorPositionEventHandler;
		GetRotationEventHandler _getRotationEventHandler;
		SetRotationEventHandler _setRotationEventHandler;
		GetScaleEventHandler _getScaleEventHandler;
		SetScaleEventHandler _setScaleEventHandler;
		CreateEntityEventHandler _createEntityEventHandler;
		DestroyEntityEventHandler _destroyEntityEventHandler;
		void* _userData;
	};
}
