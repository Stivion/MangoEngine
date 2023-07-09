cmake_minimum_required(VERSION 3.21)
project(Mango)

set(CMAKE_CXX_STANDARD 23)

# Project files
file(
        GLOB_RECURSE PROJECT_SOURCES
        Source/*.h
        Source/*.cpp
)

# Build type definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_definitions(RELEASE)
endif()

# Project executable
add_executable(
        Mango
        Source/Main.cpp
        ${PROJECT_SOURCES}
)

# Test Games
set(${PROJECT_NAME}_COPY_TEST_GAMES ON)

# Link with Vulkan
## Find Vulkan SDK
find_package(Vulkan REQUIRED) # Sets 'Vulkan_INCLUDE_DIRS' variable

## Add Vulkan
include_directories(SYSTEM ${Vulkan_INCLUDE_DIRS})

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
add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC Libraries/ImGui)
target_include_directories(ImGui PUBLIC Libraries/ImGui/backends)
## ImGui GLFW implementation needs to be linked with GLFW
target_link_libraries(ImGui glfw)

## Link with ImGui
target_link_libraries(${PROJECT_NAME} ImGui)

# Link with stb_image
include_directories(Libraries/stb)
link_directories(Libraries/stb)

# Link with EnTT
add_subdirectory(Libraries/entt)
target_link_libraries(${PROJECT_NAME} EnTT::EnTT)

# Link with Box2D
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)

add_subdirectory(Libraries/Box2D)
target_link_libraries(${PROJECT_NAME} box2d)

# Link with nlohmann/json
add_subdirectory(Libraries/json)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

# Link with Python
## Link with static library
target_include_directories(${PROJECT_NAME} PRIVATE Libraries/python/include)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python3.lib)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python311.lib)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python3_d.lib)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python311_d.lib)
else()
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python3.lib)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Libraries/python/libs/python311.lib)
endif()


## Copy DLL's
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python3.dll python3.dll COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python311.dll python311.dll COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python3_d.dll python3_d.dll COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python311_d.dll python311_d.dll COPYONLY)
else()
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python3.dll python3.dll COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/Libraries/python/dll/python311.dll python311.dll COPYONLY)
endif()

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

# Copy test games
if (DEFINED ${PROJECT_NAME}_COPY_TEST_GAMES)
    file(COPY ${PROJECT_SOURCE_DIR}/TestGames/FloppyBirb DESTINATION .)
endif()

# Copy imgui config for layout
configure_file(${PROJECT_SOURCE_DIR}/imgui.ini imgui.ini COPYONLY)
