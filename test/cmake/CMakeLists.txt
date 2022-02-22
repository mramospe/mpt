#
# Example of a CMake project depending on MPT
#
cmake_minimum_required(VERSION 2.8)
project(MPTTest)
set(CMAKE_CXX_STANDARD 20)
find_package(MPT REQUIRED)
message("-- Found MPT version ${MPT_VERSION}")
message("-- Including directories ${MPT_INCLUDE_DIR}")
add_executable(main main.cpp)
target_include_directories(main PUBLIC ${MPT_INCLUDE_DIR})
