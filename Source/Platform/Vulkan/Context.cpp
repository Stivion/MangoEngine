#include "Context.h"

#include "RenderSurface_ImplGLFW.h"
#include "../../Infrastructure/Assert/Assert.h"

Mango::Context::Context(Mango::Window* window)
	: _window(window)
{
    const Mango::GLFWWindow* glfwWindow = dynamic_cast<const Mango::GLFWWindow*>(window);
    M_ASSERT(glfwWindow != nullptr && "Only GLFW window implementations supported");

    _instance = std::make_unique<Mango::Instance>();
    _renderSurface = std::make_unique<Mango::RenderSurface_ImplGLFW>(*glfwWindow, *_instance);
    _physicalDevice = std::make_unique<Mango::PhysicalDevice>(*_instance, *_renderSurface);
    _queueFamilyIndices = std::make_unique<Mango::QueueFamilyIndices>(
        QueueFamilyIndices::FindQueueFamilies(_physicalDevice->GetDevice(), _renderSurface->GetRenderSurface())
    );
    _logicalDevice = std::make_unique<Mango::LogicalDevice>(*_physicalDevice, *_queueFamilyIndices);
}
