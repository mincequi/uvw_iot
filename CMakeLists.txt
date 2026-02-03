cmake_minimum_required(VERSION 3.5)

project(uvw_iot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:mincequi/uvw_net#bc0f376")

# Directly providing the archive URL improves performance (no git clone)
CPMAddPackage(
  NAME rpp
  URL https://github.com/victimsnino/ReactivePlusPlus/archive/refs/tags/v2.2.2.tar.gz
)
CPMAddPackage(
  NAME json
  URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz
)
CPMAddPackage(
  NAME reflect-cpp
  URL https://github.com/getml/reflect-cpp/archive/refs/tags/v0.23.0.tar.gz
)

add_subdirectory(uvw_iot)

# Add tests
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add tools
option(BUILD_TOOLS "Build tools" OFF)
if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()
