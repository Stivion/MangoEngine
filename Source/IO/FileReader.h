#pragma once

#include <fstream>
#include <vector>

namespace Mango
{
    class FileReader
    {
    public:
        FileReader() = delete;
        
        static std::vector<char> ReadFileBytes(const std::string& filepath);
    };
}
