﻿#include "FileReader.h"

#include <stdexcept>

std::vector<char> Mango::FileReader::ReadFileBytes(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    int64_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}
