#include "Application.h"

void Mango::Application::Run()
{
    InitializeWindow(800, 600);
    InitializeVulkan();
    CreateSyncObjects();
    RunMainLoop();
    Dispose();
}

void Mango::Application::InitializeWindow(uint32_t width, uint32_t height)
{
    _windowWidth = width;
    _windowHeight = height;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _window = glfwCreateWindow(static_cast<int>(_windowWidth), static_cast<int>(_windowHeight), "Learning Vulkan", nullptr, nullptr);
    
}

void Mango::Application::InitializeVulkan()
{
    _instance.CreateInstance();
    _renderSurface.CreateRenderSurface(_window, &_instance);
    _physicalDevice.PickPhysicalDevice(_instance, _renderSurface);
    _logicalDevice.CreateLogicalDevice(_physicalDevice, _renderSurface);
    _swapChain.CreateSwapChain(_window, _physicalDevice, _logicalDevice, _renderSurface);
    _imageView.CreateImageViews(_logicalDevice, _swapChain);
    _renderPass.CreateRenderPass(_logicalDevice, _swapChain);
    _graphicsPipeline.CreateGraphicsPipeline(_logicalDevice, _swapChain, _renderPass);
    _framebuffer.CreateFramebuffers(_logicalDevice, _swapChain, _imageView, _renderPass);
    _commandPool.CreateCommandPool(_physicalDevice, _logicalDevice, _renderSurface);
    _commandBuffer.CreateCommandBuffer(_logicalDevice, _swapChain, _renderPass, _graphicsPipeline, _framebuffer, _commandPool);
}

void Mango::Application::RunMainLoop()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(_logicalDevice.GetDevice());
}

void Mango::Application::DrawFrame()
{
    vkWaitForFences(_logicalDevice.GetDevice(), 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_logicalDevice.GetDevice(), 1, &_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(_logicalDevice.GetDevice(), _swapChain.GetSwapChain(), UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    vkResetCommandBuffer(_commandBuffer.GetCommandBuffer(), 0);
    _commandBuffer.RecordCommandBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer.GetCommandBuffer();

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain.GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(_logicalDevice.GetPresentationQueue(), &presentInfo);
}

void Mango::Application::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(_logicalDevice.GetDevice(), &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create sync objects");
    }
}

void Mango::Application::Dispose()
{
    vkDestroySemaphore(_logicalDevice.GetDevice(), _imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(_logicalDevice.GetDevice(), _renderFinishedSemaphore, nullptr);
    vkDestroyFence(_logicalDevice.GetDevice(), _inFlightFence, nullptr);
    
    glfwDestroyWindow(_window);
    glfwTerminate();
}
