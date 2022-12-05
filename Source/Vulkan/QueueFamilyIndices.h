#pragma once

#include <cstdint>
#include <optional>

struct QueueFamilyIndices
{
    std::optional<std::uint32_t> GraphicsFamily;
    std::optional<std::uint32_t> PresentationFamily;
};