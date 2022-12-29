#include "RenderSurface.h"

Mango::RenderSurface::RenderSurface(const Mango::Instance& instance)
    : _instance(instance.GetInstance())
{
}

Mango::RenderSurface::~RenderSurface()
{
    vkDestroySurfaceKHR(_instance, _renderSurface, nullptr);
}
