#pragma once

#include "../GUID.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

namespace Mango
{
	namespace Scripting
	{
		typedef PyObject* (*ModuleInitFunc)(void);

		class ScriptableEntity
		{
		public:
			Mango::GUID _id;
		};

		typedef struct
		{
			PyObject_HEAD
			ScriptableEntity* objPtr;
		} PyEntity;

		std::string GetLibraryName();
		ModuleInitFunc GetModuleInitializationFunction();
	}
}
