#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <iostream>

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    auto* extensions = new VkExtensionProperties[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

    std::cout << extensionCount << " extensions supported\n";
    for (int i = 0; i < extensionCount; i++) {
        std::cout << "Extension: " << extensions[i].extensionName << "\n";
    }
    delete[] extensions;

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
    
//    ImGui::CreateContext();
//    ImGui_ImplGlfw_InitForVulkan(window, true);
//    ImGui::ShowDemoWindow();
//    ImGui::DestroyContext();

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
