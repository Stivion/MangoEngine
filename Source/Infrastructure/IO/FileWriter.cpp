#include "FileWriter.h"

#include <fstream>
#include <stdexcept>

void Mango::FileWriter::WriteFile(const std::string& filepath, const std::string& text)
{
    std::ofstream file(filepath, std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    file.write(text.c_str(), text.size());
    file.close();
}

void Mango::FileWriter::WriteFile(const std::filesystem::path& filepath, const std::string& text)
{
    std::ofstream file(filepath, std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filepath.string());
    }

    file.write(text.c_str(), text.size());
    file.close();
}
