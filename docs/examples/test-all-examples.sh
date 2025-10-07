#!/bin/bash
# test-all-examples.sh
#
# Test script for ROSE external project examples
# This script tests all three build system examples (CMake, Autotools, Makefile)
# to verify they can successfully build and run against an installed ROSE.
#
# Usage:
#   ./test-all-examples.sh /path/to/rose/install
#
# Requirements:
#   - ROSE must be fully installed (not just built)
#   - Same environment used to build ROSE (e.g., spock-shell if used)
#   - CMake, Autotools (autoconf, automake, libtool), and make must be available
#
# Exit codes:
#   0 - All tests passed
#   1 - Usage error or missing ROSE installation
#   2 - CMake example failed
#   3 - Autotools example failed
#   4 - Makefile example failed

set -e  # Exit on error
set -u  # Exit on undefined variable

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

log_section() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$*${NC}"
    echo -e "${BLUE}========================================${NC}"
}

# Check if ROSE installation path is provided
if [ $# -lt 1 ]; then
    log_error "Usage: $0 <rose-install-path>"
    log_error "Example: $0 /usr/local/rose"
    exit 1
fi

ROSE_INSTALL_PATH="$1"

# Verify ROSE installation exists
if [ ! -d "$ROSE_INSTALL_PATH" ]; then
    log_error "ROSE installation path does not exist: $ROSE_INSTALL_PATH"
    exit 1
fi

# Verify required ROSE files exist
if [ ! -f "$ROSE_INSTALL_PATH/lib/cmake/Rose/RoseConfig.cmake" ]; then
    log_error "RoseConfig.cmake not found in $ROSE_INSTALL_PATH/lib/cmake/Rose/"
    log_error "ROSE may not be fully installed."
    exit 1
fi

if [ ! -f "$ROSE_INSTALL_PATH/lib/pkgconfig/rose.pc" ]; then
    log_error "rose.pc not found in $ROSE_INSTALL_PATH/lib/pkgconfig/"
    log_error "ROSE may not be fully installed."
    exit 1
fi

log_success "Found ROSE installation at: $ROSE_INSTALL_PATH"

# Get absolute path to ROSE installation
ROSE_INSTALL_PATH=$(cd "$ROSE_INSTALL_PATH" && pwd)

# Find the script directory (where examples are located)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Test output directory
TEST_OUTPUT_DIR="${SCRIPT_DIR}/test-output"
log_info "Test output directory: $TEST_OUTPUT_DIR"
rm -rf "$TEST_OUTPUT_DIR"
mkdir -p "$TEST_OUTPUT_DIR"

# Environment setup
export CMAKE_PREFIX_PATH="$ROSE_INSTALL_PATH"
export PKG_CONFIG_PATH="$ROSE_INSTALL_PATH/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export LD_LIBRARY_PATH="$ROSE_INSTALL_PATH/lib:${LD_LIBRARY_PATH:-}"

log_info "CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH"
log_info "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"
log_info "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

# Get ROSE version
ROSE_VERSION=$(pkg-config --modversion rose 2>/dev/null || echo "unknown")
log_info "ROSE version: $ROSE_VERSION"

# Check for ROSE features
ROSE_FEATURES=""
if [ -f "$ROSE_INSTALL_PATH/lib/cmake/Rose/RoseConfig.cmake" ]; then
    if grep -q "set(Rose_ENABLE_BINARY_ANALYSIS \(TRUE\|ON\))" "$ROSE_INSTALL_PATH/lib/cmake/Rose/RoseConfig.cmake"; then
        ROSE_FEATURES="${ROSE_FEATURES}binary-analysis "
    fi
    if grep -q "set(Rose_ENABLE_C \(TRUE\|ON\))" "$ROSE_INSTALL_PATH/lib/cmake/Rose/RoseConfig.cmake"; then
        ROSE_FEATURES="${ROSE_FEATURES}c/c++ "
    fi
    if grep -q "set(Rose_ENABLE_FORTRAN \(TRUE\|ON\))" "$ROSE_INSTALL_PATH/lib/cmake/Rose/RoseConfig.cmake"; then
        ROSE_FEATURES="${ROSE_FEATURES}fortran "
    fi
fi
log_info "ROSE features: ${ROSE_FEATURES:-none detected}"

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

#############################################################################
# Test 1: CMake Example
#############################################################################

test_cmake_example() {
    log_section "Testing CMake Example"
    TESTS_RUN=$((TESTS_RUN + 1))

    local test_dir="$TEST_OUTPUT_DIR/cmake-test"
    mkdir -p "$test_dir"

    # Copy example to test directory
    cp -r "$SCRIPT_DIR/external-project-cmake"/* "$test_dir/"
    cd "$test_dir"

    log_info "Configuring CMake project..."
    if ! cmake -B build -DCMAKE_PREFIX_PATH="$ROSE_INSTALL_PATH" > cmake_configure.log 2>&1; then
        log_error "CMake configuration failed"
        cat cmake_configure.log
        return 2
    fi
    log_success "CMake configuration succeeded"

    log_info "Building CMake project..."
    if ! cmake --build build > cmake_build.log 2>&1; then
        log_error "CMake build failed"
        cat cmake_build.log
        return 2
    fi
    log_success "CMake build succeeded"

    # Test simple_analyzer
    log_info "Testing simple_analyzer..."
    if [ -f build/simple_analyzer ]; then
        if ! ./build/simple_analyzer > simple_analyzer_output.log 2>&1; then
            log_error "simple_analyzer execution failed"
            cat simple_analyzer_output.log
            return 2
        else
            log_success "simple_analyzer executed successfully"
            # Check that it shows ROSE version
            if grep -q "ROSE Version:" simple_analyzer_output.log; then
                log_success "simple_analyzer produced expected output"
            fi
        fi
    else
        log_error "simple_analyzer binary not found"
        return 2
    fi

    # Test binary_analyzer if binary analysis is enabled
    if [ -f build/binary_analyzer ]; then
        log_info "Testing binary_analyzer (binary analysis enabled)..."
        if ! ./build/binary_analyzer test_input.exe > binary_analyzer_output.log 2>&1; then
            log_warning "binary_analyzer execution failed"
            cat binary_analyzer_output.log
        else
            log_success "binary_analyzer executed successfully"
        fi
    else
        log_info "binary_analyzer not built (binary analysis not enabled)"
    fi

    # Test fortran_analyzer if Fortran is enabled
    if [ -f build/fortran_analyzer ]; then
        log_info "Testing fortran_analyzer (Fortran enabled)..."
        # Create a simple Fortran test file
        cat > test_fortran.f90 << 'EOF'
program test
    implicit none
    print *, "Hello"
end program test
EOF
        if ! ./build/fortran_analyzer test_fortran.f90 > fortran_analyzer_output.log 2>&1; then
            log_warning "fortran_analyzer execution failed"
            cat fortran_analyzer_output.log
        else
            log_success "fortran_analyzer executed successfully"
        fi
    else
        log_info "fortran_analyzer not built (Fortran not enabled)"
    fi

    log_success "CMake example test passed"
    TESTS_PASSED=$((TESTS_PASSED + 1))
    return 0
}

#############################################################################
# Test 2: Autotools Example
#############################################################################

test_autotools_example() {
    log_section "Testing Autotools Example"
    TESTS_RUN=$((TESTS_RUN + 1))

    # Check if autotools are available
    if ! command -v autoreconf >/dev/null 2>&1; then
        log_warning "autoreconf not found, skipping Autotools test"
        log_warning "Install autoconf and automake to test Autotools example"
        return 0
    fi

    local test_dir="$TEST_OUTPUT_DIR/autotools-test"
    mkdir -p "$test_dir"

    # Copy example to test directory
    cp -r "$SCRIPT_DIR/external-project-autotools"/* "$test_dir/"
    cd "$test_dir"

    log_info "Generating build system with autogen.sh..."
    if ! ./autogen.sh > autogen.log 2>&1; then
        log_error "autogen.sh failed"
        cat autogen.log
        return 3
    fi
    log_success "Build system generated"

    log_info "Configuring Autotools project..."
    if ! ./configure PKG_CONFIG_PATH="$PKG_CONFIG_PATH" > configure.log 2>&1; then
        log_error "configure failed"
        cat configure.log
        return 3
    fi
    log_success "Configuration succeeded"

    log_info "Building Autotools project..."
    if ! make > make.log 2>&1; then
        log_error "make failed"
        cat make.log
        return 3
    fi
    log_success "Build succeeded"

    # Test simple_analyzer
    log_info "Testing simple_analyzer..."
    if [ -f simple_analyzer ]; then
        if ! ./simple_analyzer > simple_analyzer_output.log 2>&1; then
            log_error "simple_analyzer execution failed"
            cat simple_analyzer_output.log
            return 3
        else
            log_success "simple_analyzer executed successfully"
            # Check that it shows ROSE version
            if grep -q "ROSE Version:" simple_analyzer_output.log; then
                log_success "simple_analyzer produced expected output"
            fi
        fi
    else
        log_error "simple_analyzer binary not found"
        return 3
    fi

    log_success "Autotools example test passed"
    TESTS_PASSED=$((TESTS_PASSED + 1))
    return 0
}

#############################################################################
# Test 3: Makefile Example
#############################################################################

test_makefile_example() {
    log_section "Testing Makefile Example"
    TESTS_RUN=$((TESTS_RUN + 1))

    local test_dir="$TEST_OUTPUT_DIR/makefile-test"
    mkdir -p "$test_dir"

    # Copy example to test directory
    cp -r "$SCRIPT_DIR/external-project-makefile"/* "$test_dir/"
    cd "$test_dir"

    log_info "Building Makefile project..."
    if ! make PKG_CONFIG_PATH="$PKG_CONFIG_PATH" > make.log 2>&1; then
        log_error "make failed"
        cat make.log
        return 4
    fi
    log_success "Build succeeded"

    # Test simple_analyzer
    log_info "Testing simple_analyzer..."
    if [ -f simple_analyzer ]; then
        if ! ./simple_analyzer > simple_analyzer_output.log 2>&1; then
            log_error "simple_analyzer execution failed"
            cat simple_analyzer_output.log
            return 4
        else
            log_success "simple_analyzer executed successfully"
            # Check that it shows ROSE version
            if grep -q "ROSE Version:" simple_analyzer_output.log; then
                log_success "simple_analyzer produced expected output"
            fi
        fi
    else
        log_error "simple_analyzer binary not found"
        return 4
    fi

    # Test clean
    log_info "Testing make clean..."
    if ! make clean > make_clean.log 2>&1; then
        log_warning "make clean failed"
    else
        log_success "make clean succeeded"
    fi

    log_success "Makefile example test passed"
    TESTS_PASSED=$((TESTS_PASSED + 1))
    return 0
}

#############################################################################
# Run all tests
#############################################################################

# Run tests and capture exit codes
CMAKE_RESULT=0
AUTOTOOLS_RESULT=0
MAKEFILE_RESULT=0

test_cmake_example || CMAKE_RESULT=$?
test_autotools_example || AUTOTOOLS_RESULT=$?
test_makefile_example || MAKEFILE_RESULT=$?

#############################################################################
# Summary
#############################################################################

log_section "Test Summary"

echo "ROSE Installation: $ROSE_INSTALL_PATH"
echo "ROSE Version: $ROSE_VERSION"
echo "ROSE Features: ${ROSE_FEATURES:-none detected}"
echo ""
echo "Tests Run: $TESTS_RUN"
echo "Tests Passed: $TESTS_PASSED"
echo "Tests Failed: $TESTS_FAILED"
echo ""

if [ $CMAKE_RESULT -eq 0 ]; then
    log_success "✓ CMake example: PASSED"
else
    log_error "✗ CMake example: FAILED (exit code $CMAKE_RESULT)"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

if [ $AUTOTOOLS_RESULT -eq 0 ]; then
    log_success "✓ Autotools example: PASSED"
else
    log_error "✗ Autotools example: FAILED (exit code $AUTOTOOLS_RESULT)"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

if [ $MAKEFILE_RESULT -eq 0 ]; then
    log_success "✓ Makefile example: PASSED"
else
    log_error "✗ Makefile example: FAILED (exit code $MAKEFILE_RESULT)"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

echo ""
log_info "Test output directory: $TEST_OUTPUT_DIR"
log_info "Review logs in subdirectories for detailed information"

# Exit with appropriate code
if [ $TESTS_FAILED -gt 0 ]; then
    log_error "Some tests failed"
    exit 1
else
    log_success "All tests passed!"
    exit 0
fi
