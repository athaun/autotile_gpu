cmake_minimum_required(VERSION 3.20)
project(AutoTile)

set(CMAKE_CXX_STANDARD 20)

# Options
option(KOMPUTE_OPT_GIT_TAG "The tag of the repo to use for the example" v0.9.0)
option(KOMPUTE_OPT_FROM_SOURCE "Whether to build example from source or from git fetch repo" ON)

if(KOMPUTE_OPT_FROM_SOURCE)
    add_subdirectory(./lib/kompute ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)
else()
    include(FetchContent)
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

# Then add it to the library, so you can access it later in your code
add_library(shader INTERFACE "src/shader/my_shader.hpp")
target_include_directories(shader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Setting up main example code
file(GLOB SOURCES "src/*.cpp")
add_executable(AutoTile ${SOURCES})
target_link_libraries(AutoTile PRIVATE shader kompute::kompute)

# Copy input folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/input
     DESTINATION ${CMAKE_BINARY_DIR}/)
