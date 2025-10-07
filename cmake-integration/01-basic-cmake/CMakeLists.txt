cmake_minimum_required(VERSION 3.15)
project(BasicRoseIntegrationTest CXX)

# Test basic find_package(Rose)
find_package(Rose REQUIRED)

message(STATUS "Test: Found Rose version ${Rose_VERSION}")

# Verify essential variables are set
if(NOT DEFINED Rose_VERSION)
    message(FATAL_ERROR "Rose_VERSION not defined")
endif()

if(NOT TARGET Rose::rose)
    message(FATAL_ERROR "Rose::rose target not found")
endif()

message(STATUS "Test: Rose::rose target exists")

# Create a minimal test executable
add_executable(test_basic_rose test_basic_rose.cpp)
target_link_libraries(test_basic_rose PRIVATE Rose::rose)

# Optional: Test that we can query feature flags
if(DEFINED Rose_ENABLE_BINARY_ANALYSIS)
    message(STATUS "Test: Binary analysis enabled: ${Rose_ENABLE_BINARY_ANALYSIS}")
endif()

message(STATUS "Test: Basic CMake integration test configured successfully")
