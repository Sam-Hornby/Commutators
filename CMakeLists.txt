
cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
project(operators)
enable_testing()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(COMPILER_IS_GCC_COMPATIBLE ON)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(COMPILER_IS_GCC_COMPATIBLE ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(COMPILER_IS_GCC_COMPATIBLE)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-std=c++17" SUPPORTS_CXX17)
  if(SUPPORTS_CXX17)
    message(STATUS "-std=c++17 is supported.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_CXX_STANDARD 17)
  else()
    message(FATAL_ERROR "Compiler c++17 problems")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_BUILD_TESTING OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(mimalloc_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mimalloc/cmake")
set(MI_OVERRIDE OFF CACHE BOOL "" FORCE)
set(MI_OSX_INTERPOSE OFF CACHE BOOL "" FORCE)
set(MI_OSX_ZONE OFF CACHE BOOL "" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

include(GNUInstallDirs)

# Subdirectories that have warnings
add_subdirectory(abseil-cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#sub directories without warnings
add_subdirectory(pybind11)
add_subdirectory(spdlog)
add_subdirectory(benchmark)
add_subdirectory(mimalloc)

set(LIB_LINK_LIBRARIES
      absl::base
      absl::synchronization
      absl::strings
      absl::utility
      absl::memory
      absl::hash
      absl::algorithm
      absl::meta
      absl::numeric
      absl::flat_hash_map
      absl::flat_hash_set
      absl::inlined_vector
      mimalloc-static)

# My subdirectories
add_subdirectory(lib)
add_subdirectory(python)
add_subdirectory(tests)
