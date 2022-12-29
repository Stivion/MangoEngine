#pragma once

#include <random>
#include <limits>

namespace Mango
{
	class GUID
	{
	public:		
		static uint64_t GetNext();

	public:
		operator uint64_t() { return _id; }
		operator const uint64_t() const { return _id; }

	public:
		GUID();
		GUID(uint64_t id);

	private:
		static std::random_device _randomDevice;
		static std::mt19937_64 _generator;
		static std::uniform_int_distribution<unsigned long long> _distribution;

	private:
		const uint64_t _id;
	};
}
