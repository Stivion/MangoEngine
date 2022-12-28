#include "GUID.h"

std::random_device Mango::GUID::_randomDevice{};
std::mt19937_64 Mango::GUID::_generator{ _randomDevice() };
std::uniform_int_distribution<uint64_t> Mango::GUID::_distribution{ 0, std::numeric_limits<uint64_t>::max() };

uint64_t Mango::GUID::GetNext()
{
	return _distribution(_generator);
}

Mango::GUID::GUID() : _id(_distribution(_generator))
{
}

Mango::GUID::GUID(uint64_t id) : _id(id)
{
}
