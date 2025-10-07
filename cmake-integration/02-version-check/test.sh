#!/bin/bash
# Test 02: Version compatibility checking

set -e  # Exit on error

echo "================================"
echo "Test 02: Version Compatibility"
echo "================================"

if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo "ERROR: CMAKE_PREFIX_PATH not set."
    exit 1
fi

# Clean any previous build
rm -rf build
mkdir -p build

echo ""
echo "Test 2a: Finding Rose without version constraint..."
cd build
cmake .. -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" > config.log 2>&1
if grep -q "Test: Found Rose version" config.log; then
    FOUND_VERSION=$(grep "Test: Found Rose version" config.log | sed 's/.*version //')
    echo "✅ Found ROSE version: $FOUND_VERSION"
else
    echo "❌ Could not find Rose version in output"
    cat config.log
    exit 1
fi
cd ..
rm -rf build
mkdir -p build

echo ""
echo "Test 2b: Building test program (validates version information works)..."
cd build
cmake .. -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH"
cmake --build .
echo "✅ Build succeeded"
cd ..

echo ""
echo "Test 2c: Running test program..."
cd build
./test_version
echo "✅ Test program executed successfully"
cd ..
rm -rf build

echo ""
echo "✅ Test 02 PASSED: Version compatibility checks work correctly"
