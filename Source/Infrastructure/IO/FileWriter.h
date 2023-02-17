#pragma once

#include <filesystem>
#include <string>

namespace Mango
{
	class FileWriter
	{
	public:
		FileWriter() = delete;

		/// <summary>
		/// Write text to specified file path. File is created if it doesn't exist.
		/// File contents would be overriden.
		/// </summary>
		/// <param name="filepath">Path to file</param>
		/// <param name="text">Text to write to file</param>
		static void WriteFile(const std::string& filepath, const std::string& text);

		/// <summary>
		/// Write text to specified file path. File is created if it doesn't exist.
		/// File contents would be overriden.
		/// </summary>
		/// <param name="filepath">Path to file</param>
		/// <param name="text">Text to write to file</param>
		static void WriteFile(const std::filesystem::path& filepath, const std::string& text);
	};
}
