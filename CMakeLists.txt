
cmake_minimum_required(VERSION 3.6.0)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
project(operators)
enable_testing()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(COMPILER_IS_GCC_COMPATIBLE ON)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(COMPILER_IS_GCC_COMPATIBLE ON)
endif()


if(COMPILER_IS_GCC_COMPATIBLE)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-std=c++17" SUPPORTS_CXX17)
  if(SUPPORTS_CXX17)
    message(STATUS "-std=c++17 is supported.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -Werror")
  else()
    message(FATAL_ERROR "Compiler c++17 problems")
  endif()
endif()


include(GNUInstallDirs)

add_subdirectory(spdlog)
add_subdirectory(lib)
add_subdirectory(tests)
