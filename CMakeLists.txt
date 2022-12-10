cmake_minimum_required(VERSION 3.24)
project(Mango)

set(CMAKE_CXX_STANDARD 23)

# Project files
file(
        GLOB PROJECT_SOURCES
        Source/*.h
        Source/*.cpp
        Source/**/*.h
        Source/**/*.cpp
)

# Enable debug
add_compile_definitions(DEBUG)

# Project executable
add_executable(
        Mango
        Source/Main.cpp
        ${PROJECT_SOURCES}
)

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
target_include_directories("ImGui" PRIVATE Libraries/ImGui)
target_include_directories("ImGui" PRIVATE Libraries/ImGui/backends)
## ImGui GLFW implementation needs to be linked with GLFW
target_link_libraries("ImGui" glfw)

## Link with ImGui
target_link_libraries(${PROJECT_NAME} ImGui)

# Compile shaders
file(
        GLOB VERTEX_SHADERS
        Source/Shaders/*.vert
        Source/Shaders/**/*.vert
)

file(
        GLOB FRAGMENT_SHADERS
        Source/Shaders/*.frag
        Source/Shaders/**/*.frag
)

foreach(vertexShader ${VERTEX_SHADERS})
    add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND glslc -fshader-stage=vert ${vertexShader} -o vert.spv
    )
endforeach()

foreach(fragmentShader ${FRAGMENT_SHADERS})
    add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND glslc -fshader-stage=frag ${fragmentShader} -o frag.spv
    )
endforeach()
