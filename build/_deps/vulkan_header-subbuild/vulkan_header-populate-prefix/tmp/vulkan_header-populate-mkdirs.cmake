# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-src"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-build"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/tmp"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/src/vulkan_header-populate-stamp"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/src"
  "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/src/vulkan_header-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/src/vulkan_header-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/asher/code/work/autotile_gpu/build/_deps/vulkan_header-subbuild/vulkan_header-populate-prefix/src/vulkan_header-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
