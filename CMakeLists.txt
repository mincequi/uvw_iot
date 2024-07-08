cmake_minimum_required(VERSION 3.5)

project(uvw_iot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

#CPMAddPackage("gh:mincequi/uvw_net@0.1.0")
CPMAddPackage("gh:mincequi/uvw_net#8551d4c")
CPMAddPackage("gh:victimsnino/ReactivePlusPlus@2.1.1")
#CPMAddPackage("gh:victimsnino/ReactivePlusPlus#bebc5b8")
CPMAddPackage("gh:nlohmann/json@3.11.3")

add_subdirectory(uvw_iot)
add_subdirectory(tools)
# Add tests
enable_testing()
add_subdirectory(tests)
