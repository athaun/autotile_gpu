cmake_minimum_required(VERSION 3.20)
project(AutoTile)

set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS_DEBUG "-g")

include(FetchContent)

# SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# ImGui
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

# ImGui-SFML
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)

# FMT
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.1.4
)
FetchContent_MakeAvailable(fmt)

# Kompute
option(KOMPUTE_OPT_GIT_TAG "The tag of the repo to use for the example" v0.9.0)
option(KOMPUTE_OPT_FROM_SOURCE "Whether to build example from source or from git fetch repo" ON)
if(KOMPUTE_OPT_FROM_SOURCE)
    add_subdirectory(./lib/kompute ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)
else()
    FetchContent_Declare(kompute GIT_REPOSITORY https://github.com/KomputeProject/kompute.git
        GIT_TAG ${KOMPUTE_OPT_GIT_TAG})
    FetchContent_MakeAvailable(kompute)
    include_directories(${kompute_SOURCE_DIR}/src/lib)
endif()

# Compiling shader
# To add more shaders simply copy the vulkan_compile_shader command and replace it with your new shader
vulkan_compile_shader(
    INFILE src/shader/my_shader.comp
    OUTFILE src/shader/my_shader.hpp
    NAMESPACE "shader")
add_library(shader INTERFACE "src/shader/my_shader.hpp")

# Include directories for the shader
target_include_directories(shader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${SFML_SOURCE_DIR}/include)

# Setting up main example code
file(GLOB SOURCES "src/*.cpp")
add_executable(AutoTile ${SOURCES})
target_link_libraries(AutoTile PRIVATE shader kompute::kompute fmt::fmt sfml-graphics sfml-window sfml-system ImGui-SFML::ImGui-SFML)

# Copy input folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/input
     DESTINATION ${CMAKE_BINARY_DIR}/)
