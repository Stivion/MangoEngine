#include "RenderSurface_ImplGLFW.h"

#include "../../Infrastructure/Assert/Assert.h"
#include "../../Infrastructure/Logging/Logging.h"

#include <string>

Mango::RenderSurface_ImplGLFW::RenderSurface_ImplGLFW(const Mango::GLFWWindow& window, const Mango::Instance& instance)
    : RenderSurface(instance)
{
    const auto createWindowSurfaceResult = glfwCreateWindowSurface(_instance, window.GetGLFWWindow(), nullptr, &_renderSurface);
    M_TRACE("Create window surface result is: " + std::to_string(createWindowSurfaceResult));
    M_ASSERT(createWindowSurfaceResult == VK_SUCCESS && "Failed to create render surface");
}
