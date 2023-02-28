#pragma once

#include "../GUID.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <unordered_map>
#include <filesystem>

namespace Mango
{
	class ScriptEngine
	{
	public:
		ScriptEngine() = delete;
		ScriptEngine(std::unordered_map<Mango::GUID, std::filesystem::path> entitiesToScriptsMap);
		~ScriptEngine();
	};
}
