cmake_minimum_required(VERSION 3.5)

project(uvw_iot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

add_subdirectory(uvw_iot)
add_subdirectory(tools)
