#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace Mango
{
    class FileReader
    {
    public:
        FileReader() = delete;
        
        static std::vector<char> ReadFileBytes(const std::string& filepath);
        static std::string ReadAllText(const std::string& filepath);
        static std::string ReadAllText(const std::filesystem::path& filepath);
    };
}
