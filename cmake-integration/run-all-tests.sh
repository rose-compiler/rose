#!/bin/bash
# Master test runner for ROSE CMake integration tests
# This script runs all integration tests to verify the CMake modernization

set -e  # Exit on error

# Color output helpers
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "ROSE CMake Integration Test Suite"
echo "========================================"
echo ""

# Check prerequisites
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo -e "${RED}ERROR: CMAKE_PREFIX_PATH not set.${NC}"
    echo "Please set CMAKE_PREFIX_PATH to point to your ROSE installation."
    echo ""
    echo "Example:"
    echo "  export CMAKE_PREFIX_PATH=/path/to/rose/install"
    echo "  $0"
    exit 1
fi

echo "Using ROSE installation: $CMAKE_PREFIX_PATH"
echo ""

# Track test results
PASSED=0
FAILED=0
TESTS=()

# Function to run a single test
run_test() {
    local test_dir=$1
    local test_name=$(basename "$test_dir")

    echo "========================================"
    echo "Running: $test_name"
    echo "========================================"

    cd "$test_dir"

    if ./test.sh; then
        echo -e "${GREEN}✅ PASSED: $test_name${NC}"
        PASSED=$((PASSED + 1))
        TESTS+=("✅ $test_name")
    else
        echo -e "${RED}❌ FAILED: $test_name${NC}"
        FAILED=$((FAILED + 1))
        TESTS+=("❌ $test_name")
    fi

    cd - > /dev/null
    echo ""
}

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Run all tests in order
run_test "$SCRIPT_DIR/01-basic-cmake"
run_test "$SCRIPT_DIR/02-version-check"
run_test "$SCRIPT_DIR/03-pkgconfig"

# Print summary
echo "========================================"
echo "Test Summary"
echo "========================================"
for test_result in "${TESTS[@]}"; do
    echo "$test_result"
done
echo ""
echo "Total: $((PASSED + FAILED)) tests"
echo -e "${GREEN}Passed: $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
else
    echo "Failed: $FAILED"
fi
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}All tests passed!${NC}"
    echo -e "${GREEN}========================================${NC}"
    exit 0
else
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}Some tests failed!${NC}"
    echo -e "${RED}========================================${NC}"
    exit 1
fi
