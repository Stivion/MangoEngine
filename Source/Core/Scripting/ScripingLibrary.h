#pragma once

#include "../GUID.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

namespace Mango
{
	namespace Scripting
	{
		class ScriptableEntity
		{
		public:
			Mango::GUID _id = 0;
		};

		typedef struct
		{
			PyObject_HEAD
			ScriptableEntity* objPtr;
		} PyEntity;

		struct ScriptEvent
		{
			Mango::Scripting::ScriptableEntity* ScriptableEntity;
			std::string EventName;
			PyObject* Args;
		};

		typedef PyObject* (*ModuleInitFunc)(void);
		typedef PyObject* (*ScriptEventHandler)(Mango::Scripting::ScriptEvent);

		PyObject* GetEntityType();
		std::string GetLibraryName();
		ModuleInitFunc GetModuleInitializationFunction();
		void SetScriptEventHanlder(ScriptEventHandler handler);
		void SetUserPointer(void* pointer);
		void* GetUserPointer();
	}
}
