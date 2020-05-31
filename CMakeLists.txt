cmake_minimum_required(VERSION 3.10)
project(nettools)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CodeCoverage)

  set(CMAKE_CXX_FLAGS
      "-g -O0 -fprofile-arcs -ftest-coverage -Wall -Wextra -pedantic -Werror")
  set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -Wall -Wextra")
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
