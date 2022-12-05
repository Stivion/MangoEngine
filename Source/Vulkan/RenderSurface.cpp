#include "RenderSurface.h"

#include <stdexcept>

Mango::RenderSurface::~RenderSurface()
{
    vkDestroySurfaceKHR(_instance->GetInstance(), _renderSurface, nullptr);
}

void Mango::RenderSurface::CreateRenderSurface(GLFWwindow* window, Mango::Instance* instance)
{
    _instance = instance;
    if (glfwCreateWindowSurface(instance->GetInstance(), window, nullptr, &_renderSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render surface");
    }
}
