cmake_minimum_required(VERSION 3.10)

# set the project name
project(Cpptests)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add the executable
add_executable(Cpptests main.cpp window.cpp window.h)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw-3.3.6)
include_directories(lib/glfw-3.3.6/include)

# GLM
add_subdirectory(lib/glm)

# Vulkan. For now assume the sdkis installed in the system
find_package(Vulkan REQUIRED)


target_link_libraries(Cpptests PUBLIC glfw glm Vulkan::Vulkan)
