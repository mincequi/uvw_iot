# tests/CMakeLists.txt

# Add test executable
add_executable(tests RppUvwTest.cpp) # Add all your test files here

# Link Catch2
target_link_libraries(tests
PRIVATE
  Catch2::Catch2WithMain
  uvw_iot
)

# Register tests
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(tests)
