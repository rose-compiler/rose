#!/bin/bash
# Test 03: pkg-config integration for non-CMake build systems

set -e  # Exit on error

echo "============================"
echo "Test 03: pkg-config Integration"
echo "============================"

# Determine where the ROSE pkg-config file should be
if [ -z "$PKG_CONFIG_PATH" ]; then
    if [ -n "$CMAKE_PREFIX_PATH" ]; then
        # Try to construct PKG_CONFIG_PATH from CMAKE_PREFIX_PATH
        export PKG_CONFIG_PATH="$CMAKE_PREFIX_PATH/lib/pkgconfig:$CMAKE_PREFIX_PATH/lib64/pkgconfig"
        echo "Set PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
    else
        echo "ERROR: Neither PKG_CONFIG_PATH nor CMAKE_PREFIX_PATH is set."
        echo "Please set one of these to point to the ROSE installation."
        exit 1
    fi
fi

echo ""
echo "Test 3a: Check if rose.pc exists..."
if pkg-config --exists rose; then
    echo "✅ rose.pc found"
else
    echo "❌ rose.pc not found"
    echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
    exit 1
fi

echo ""
echo "Test 3b: Query ROSE version via pkg-config..."
ROSE_VERSION=$(pkg-config --modversion rose)
echo "ROSE version: $ROSE_VERSION"

echo ""
echo "Test 3c: Query ROSE cflags..."
ROSE_CFLAGS=$(pkg-config --cflags rose)
echo "ROSE cflags: $ROSE_CFLAGS"

echo ""
echo "Test 3d: Query ROSE libs..."
ROSE_LIBS=$(pkg-config --libs rose)
echo "ROSE libs: $ROSE_LIBS"

echo ""
echo "Test 3e: Build test program using Makefile with pkg-config..."
make clean 2>/dev/null || true
make

echo ""
echo "Test 3f: Run test program..."
# Set LD_LIBRARY_PATH if needed
if [ -n "$CMAKE_PREFIX_PATH" ]; then
    export LD_LIBRARY_PATH="$CMAKE_PREFIX_PATH/lib:$CMAKE_PREFIX_PATH/lib64:$LD_LIBRARY_PATH"
fi
./test_pkgconfig

echo ""
echo "Cleaning up..."
make clean

echo ""
echo "✅ Test 03 PASSED: pkg-config integration works correctly"
