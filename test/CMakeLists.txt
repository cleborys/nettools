include(ExternalProject)
include(GoogleTest)

# Download gtest
ExternalProject_Add(
  gtest
  URL https://github.com/google/googletest/archive/release-1.8.0.zip
  URL_MD5 adfafc8512ab65fd3cf7955ef0100ff5
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  # Disable install step
  INSTALL_COMMAND "")
ExternalProject_Get_Property(gtest source_dir binary_dir)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(
  libgtest
  PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
             "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
set_target_properties(
  libgmock
  PROPERTIES "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
             "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

find_package(Threads)

set(test_files
    ICMPTest.cpp
    IPPacketTest.cpp
    DNSTest.cpp
    DNSQuerierTest.cpp
    FieldElementTest.cpp
    TracerouteIntegrationTest.cpp
    TLSTest.cpp
    TCPSocketTest.cpp
    TCPServerSocketTest.cpp
    X25519Test.cpp)

add_executable(tests ${test_files} main.cpp)
target_link_libraries(tests libgtest libgmock ${CMAKE_THREAD_LIBS_INIT}
                      libnettools)

target_include_directories(
  tests
  PRIVATE "${source_dir}/googletest/include" "${source_dir}/googlemock/include"
          "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/test")
gtest_discover_tests(tests)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  setup_target_for_coverage(coverage tests coverage)
endif()
