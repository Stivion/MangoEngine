#define _CRT_SECURE_NO_WARNINGS
#include "NameComponent.h"

#include "../../Infrastructure/Assert/Assert.h"

#include <utility>

uint64_t Mango::NameComponent::_count = 0;

Mango::NameComponent::NameComponent()
{
	SetName("Entity " + std::to_string(_count));
	_count++;
}

Mango::NameComponent::NameComponent(std::string name)
{
	SetName(name);
}

Mango::NameComponent::~NameComponent()
{
	free(_buffer);
}

void Mango::NameComponent::SetName(std::string name)
{
	void* rawBuffer = malloc(_bufferSize);
	if (rawBuffer == nullptr)
	{
		free(rawBuffer);
		M_ASSERT(false && "Unable to allocate memory for NameComponent buffer");
	}

	_buffer = (char*)rawBuffer;
	memset(_buffer, 0, 128);

	auto copyCharsCount = std::min(name.size() + 1, static_cast<size_t>(127));
	strncat(_buffer, name.c_str(), copyCharsCount);
}
