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

		bool Open(std::filesystem::path* path) const
		{
#ifdef WIN32
			WindowsFileDialog winFileDialog;
			std::wstring filePathString;
			bool succeded = winFileDialog.Open(&filePathString);
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

		bool Save(std::filesystem::path* path) const
		{
#ifdef WIN32
			WindowsFileDialog winFileDialog;
			std::wstring filePathString;
			bool succeded = winFileDialog.Save(&filePathString);
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
