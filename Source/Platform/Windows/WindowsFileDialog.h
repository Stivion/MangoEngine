#pragma once

#include <vector>
#include <string>

namespace Mango
{
	class WindowsFileDialog
	{
	public:
		bool Open(std::wstring* path, std::vector<std::pair<std::wstring, std::wstring>> fileTypes) const;
		bool Save(std::wstring* path, std::vector<std::pair<std::wstring, std::wstring>> fileTypes, std::wstring defaultFileExtension) const;
	};
}
