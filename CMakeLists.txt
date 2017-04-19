# MPark.Format
#
# Copyright Michael Park, 2014-2017
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.6.3)

project(MPark.Format VERSION 1.0.0 LANGUAGES CXX)

# Option.
option(MPARK_FORMAT_INCLUDE_TESTS "Build tests." OFF)

# Target.
add_library(mpark_format INTERFACE)
target_include_directories(mpark_format INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# Config.
include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/mpark_format-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/mpark_format-config.cmake"
  INSTALL_DESTINATION lib/cmake/mpark_format
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/mpark_format-config-version.cmake"
  COMPATIBILITY AnyNewerVersion)

# Export.
export(
  TARGETS mpark_format
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/mpark_format-targets.cmake")

# Install.
install(TARGETS mpark_format EXPORT mpark_format)

install(
  EXPORT mpark_format
  FILE mpark_format-targets.cmake
  DESTINATION lib/cmake/mpark_format)

install(DIRECTORY include/mpark DESTINATION include)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/mpark_format-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/mpark_format-config-version.cmake"
  DESTINATION lib/cmake/mpark_format)

if(MPARK_FORMAT_INCLUDE_TESTS)
  enable_testing()
  add_subdirectory(3rdparty/googletest/googletest)
  add_subdirectory(test)
endif()
