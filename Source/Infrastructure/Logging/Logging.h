#pragma once

#include <iostream>

namespace Mango
{

#define M_TRACE(message) std::cout << "[TRACE]: " << message << std::endl;
#define M_DEBUG(message) std::cout << "[DEBUG]: " << message << std::endl;
#define M_INFO(message) std::cout << "[INFO]: " << message << std::endl;
#define M_WARN(message) std::cout << "[WARN]: " << message << std::endl;
#define M_ERROR(message) std::cout << "[ERROR]: " << message << std::endl;
#define M_CRITICAL(message) std::cout << "[CRIT]: " << message << std::endl;

}
