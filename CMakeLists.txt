cmake_minimum_required(VERSION 3.24)
project(Mango)

set(CMAKE_CXX_STANDARD 23)

# Project executable
add_executable(Mango Source/main.cpp)

# Link with Vulkan
## Find Vulkan SDK
find_package(Vulkan REQUIRED) # Sets 'Vulkan_INCLUDE_DIRS' variable

## Add Vulkan
include_directories(${Vulkan_INCLUDE_DIRS})

## Link Vulkan
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# Link with GLM
## Add GLM
add_subdirectory(Libraries/glm)

## Link with GLM
target_link_libraries(${PROJECT_NAME} glm)

# Link with GLFW
## Disable docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

## Add GLFW
add_subdirectory(Libraries/glfw)

## Link GLFW
target_link_libraries(${PROJECT_NAME} glfw)

# Link with ImGui
## Compile as static library
file(
        GLOB IMGUI_SOURCES
        Libraries/ImGui/*.cpp
        Libraries/ImGui/*.h
        Libraries/ImGui/backends/imgui_impl_vulkan.h
        Libraries/ImGui/backends/imgui_impl_vulkan.cpp
        Libraries/ImGui/backends/imgui_impl_glfw.h
        Libraries/ImGui/backends/imgui_impl_glfw.cpp
)
add_library("ImGui" ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC Libraries/ImGui)
target_include_directories("ImGui" PUBLIC Libraries/ImGui/backends)
## ImGui GLFW implementation needs to be linked with GLFW
target_link_libraries("ImGui" glfw)

## Link with ImGui
target_link_libraries(${PROJECT_NAME} ImGui)
