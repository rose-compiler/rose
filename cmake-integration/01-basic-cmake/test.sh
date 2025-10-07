#!/bin/bash
# Test 01: Basic CMake find_package(Rose) integration

set -e  # Exit on error

echo "================================"
echo "Test 01: Basic CMake Integration"
echo "================================"

# Check if CMAKE_PREFIX_PATH is set (required to find Rose)
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo "ERROR: CMAKE_PREFIX_PATH not set. Please set it to point to ROSE installation."
    echo "Example: export CMAKE_PREFIX_PATH=/path/to/rose/install"
    exit 1
fi

# Clean any previous build
rm -rf build
mkdir build
cd build

echo "Configuring with CMake..."
cmake .. -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"

echo "Building test..."
cmake --build .

echo "Running test executable..."
./test_basic_rose

echo ""
echo "✅ Test 01 PASSED: Basic CMake integration works"
