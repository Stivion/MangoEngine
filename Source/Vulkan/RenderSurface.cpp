#include "RenderSurface.h"

#include "../Infrastructure/Assert/Assert.h"
#include "../Infrastructure/Logging/Logging.h"

Mango::RenderSurface::RenderSurface(Mango::Window& window, Mango::Instance& instance) 
    : _window(window.GetWindow()), _instance(instance.GetInstance())
{
    const auto createWindowSurfaceResult = glfwCreateWindowSurface(_instance, _window, nullptr, &_renderSurface);
    M_TRACE("Create window surface result is: " + createWindowSurfaceResult);
    M_ASSERT(createWindowSurfaceResult == VK_SUCCESS && "Failed to create render surface");
}

Mango::RenderSurface::~RenderSurface()
{
    vkDestroySurfaceKHR(_instance, _renderSurface, nullptr);
}
