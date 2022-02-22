cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

option(INSTALL_TESTS "Whether to install the test scripts or not" OFF)

set(PROJECT_LANGUAGES CXX)

project("MPT" VERSION 0.1.0 DESCRIPTION "A header only c++ library with utilities for metaprogramming"
  LANGUAGES ${PROJECT_LANGUAGES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10)
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10)
    message(FATAL_ERROR "Insufficient clang version")
  endif()
else()
  message(WARNING "Not sure about the features of the $CMAKE_CXX_COMPILER_ID compiler; unforeseen errors might appear")
endif()

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
$<INSTALL_INTERFACE:include>)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

configure_package_config_file(
 "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
 "${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
 INSTALL_DESTINATION
 ${PROJECT_BINARY_DIR}/cmake)

export(EXPORT ${PROJECT_NAME}Targets
  FILE "${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
  )

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION ${PROJECT_BINARY_DIR} FILES_MATCHING PATTERN *.hpp)

#
# Avoid incorrect warnings present in Clang and not in GCC
#
# See:
# https: // bugs.llvm.org/show_bug.cgi?id=21629
# https: // stackoverflow.com/questions/13905200/is-it-wise-to-ignore-gcc-clangs-wmissing-braces-warning
#
if(INSTALL_TESTS)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
     add_compile_options(-Wno-missing-braces)
  endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
endif(INSTALL_TESTS)

#
# Installation of the test scripts
#
if(INSTALL_TESTS)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/cpp)
    include_directories(include test/include)

    file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/cpp/*.cpp)

    set(CMAKE_CXX_FLAGS "-O3 -pedantic -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -pedantic -Wall -Wextra")

    foreach(testsourcefile ${TEST_SOURCES})

      get_filename_component(testname ${testsourcefile} NAME_WE)
      add_executable(${testname} ${testsourcefile})
      set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/cpp CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)

    endforeach(testsourcefile ${TEST_SOURCES})

endif(INSTALL_TESTS)
