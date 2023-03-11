#define _CRT_SECURE_NO_WARNINGS
#include "ScriptComponent.h"

#include "../../Infrastructure/Assert/Assert.h"

#include <utility>

Mango::ScriptComponent::ScriptComponent()
{
	std::string scriptName = "";
	SetFileName(scriptName);
}

Mango::ScriptComponent::~ScriptComponent()
{
	free(_buffer);
}

void Mango::ScriptComponent::SetFileName(std::string& fileName)
{
	void* rawBuffer = malloc(_bufferSize);
	if (rawBuffer == nullptr)
	{
		free(rawBuffer);
		M_ASSERT(false && "Unable to allocate memory for ScriptComponent buffer");
	}

	_buffer = (char*)rawBuffer;
	memset(_buffer, 0, 128);

	auto copyCharsCount = std::min(fileName.size() + 1, static_cast<size_t>(127));
	strncat(_buffer, fileName.c_str(), copyCharsCount);
}
