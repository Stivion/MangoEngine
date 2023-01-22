#pragma once

#include <cstdint>
#include <string>

namespace Mango
{
	class NameComponent
	{
	public:
		NameComponent();
		NameComponent(std::string name);
		~NameComponent();

		inline char* GetName() { return _buffer; }
		inline char* GetName() const { return _buffer; }
		void SetName(std::string name);
		uint32_t GetBufferSize() { return _bufferSize; }

	private:
		static uint64_t _count;
		const uint32_t _bufferSize = 128;
		char* _buffer;
	};
}
