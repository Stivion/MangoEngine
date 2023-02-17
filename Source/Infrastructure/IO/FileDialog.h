#pragma once

#include "../../Platform/Windows/WindowsFileDialog.h"

#include <filesystem>
#include <string>
#include <stdexcept>

namespace Mango
{
	class FileDialog
	{
	public:
		FileDialog() = default;

		bool Open(std::filesystem::path* path, std::vector<std::pair<std::wstring, std::wstring>> fileTypes) const
		{
#ifdef WIN32
			WindowsFileDialog winFileDialog;
			std::wstring filePathString;
			bool succeded = winFileDialog.Open(&filePathString, fileTypes);
			if (!succeded)
			{
				return false;
			}

			std::filesystem::path filePath(filePathString);
			*path = filePath;
			return true;
#endif
			throw std::runtime_error("Current platform is not supported");
		}

		bool Save(std::filesystem::path* path, std::vector<std::pair<std::wstring, std::wstring>> fileTypes) const
		{
			return Save(path, fileTypes, L"");
		}

		bool Save(std::filesystem::path* path, std::vector<std::pair<std::wstring, std::wstring>> fileTypes, std::wstring defaultFileExtension) const
		{
#ifdef WIN32
			WindowsFileDialog winFileDialog;
			std::wstring filePathString;
			bool succeded = winFileDialog.Save(&filePathString, fileTypes, defaultFileExtension);
			if (!succeded)
			{
				return false;
			}

			std::filesystem::path filePath(filePathString);
			*path = filePath;
			return true;
#endif
			throw std::runtime_error("Current platform is not supported");
		}
	};
}
