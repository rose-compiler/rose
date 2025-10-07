cmake_minimum_required(VERSION 3.15)
project(VersionCheckRoseTest CXX)

# Test 1: Find any version of Rose
find_package(Rose REQUIRED)
message(STATUS "Test: Found Rose version ${Rose_VERSION}")

# Parse version components
if(Rose_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
    set(ROSE_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(ROSE_VERSION_MINOR ${CMAKE_MATCH_2})
    set(ROSE_VERSION_PATCH ${CMAKE_MATCH_3})
    message(STATUS "Test: Parsed version: ${ROSE_VERSION_MAJOR}.${ROSE_VERSION_MINOR}.${ROSE_VERSION_PATCH}")
endif()

# Test 2: Version compatibility checking
# Note: The actual version constraint would be tested by the test.sh script
# by running multiple cmake invocations with different version requirements

if(NOT TARGET Rose::rose)
    message(FATAL_ERROR "Rose::rose target not found")
endif()

# Create test executable
add_executable(test_version test_version.cpp)
target_link_libraries(test_version PRIVATE Rose::rose)

message(STATUS "Test: Version check test configured successfully")
