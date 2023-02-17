#pragma once

#include <string>

namespace Mango
{
	class WindowsFileDialog
	{
	public:
		bool Open(std::wstring* path) const;
		bool Save(std::wstring* path) const;
	};
}
