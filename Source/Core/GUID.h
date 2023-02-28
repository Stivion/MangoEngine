#pragma once

#include <random>
#include <limits>

namespace Mango
{
	class GUID
	{
	public:		
		static uint64_t GetNext();
		static uint64_t Empty() { return 0; }

	public:
		operator uint64_t() { return _id; }
		operator const uint64_t() const { return _id; }

	public:
		GUID();
		GUID(uint64_t id);
		GUID& operator=(const GUID&) = default;

	private:
		static std::random_device _randomDevice;
		static std::mt19937_64 _generator;
		static std::uniform_int_distribution<unsigned long long> _distribution;

	private:
		uint64_t _id;
	};
}

namespace std
{
	template<>
	class hash<Mango::GUID>
	{
	public:
		std::uint64_t operator()(const Mango::GUID& guid) const
		{
			std::hash<uint64_t> hash;
			return hash(guid);
		}
	};
}
