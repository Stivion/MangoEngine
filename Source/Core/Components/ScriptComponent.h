#pragma once

#include <cstdint>
#include <string>

namespace Mango
{
	class ScriptComponent
	{
	public:
		ScriptComponent();
		~ScriptComponent();

		inline char* GetFileName() { return _buffer; }
		uint32_t GetBufferSize() { return _bufferSize; }

		void SetFileName(std::string& fileName);

	private:
		const uint32_t _bufferSize = 128;
		char* _buffer;
	};
}
