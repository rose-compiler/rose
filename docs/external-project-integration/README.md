# External Project Integration Guide

This document describes how to integrate ROSE into your external
project using various build systems. Details depend on which build
system your project uses:

* [CMake](integration-guide-cmake.md)
* [GNU Autotools](integration-guide-autotools.md)
* [GNU Make](integration-guide-make.md)

# Examples

Full examples are in subdirectories, which also serve as tests. Each
directory contains a README.md that describes how to run the test.

## 1. CMake (`external-project-cmake/`)

Modern CMake example with:
- `find_package(Rose)` integration
- Feature detection (binary analysis, Fortran, etc.)
- Multiple conditional programs
- Based on ROSE's bat-lsv.C for binary analysis

## 2. Autotools (`external-project-autotools/`)

GNU Autotools example with:
- `PKG_CHECK_MODULES` for ROSE detection
- Standard Autoconf/Automake setup
- pkg-config integration

## 3. Makefile (`external-project-makefile/`)

Plain Makefile example with:
- pkg-config for compiler/linker flags
- Simple and straightforward
- No configuration step needed

# Testing All Examples

A comprehensive test script is provided to validate all examples:

## Usage

```bash
./test-all-examples.sh /path/to/rose/install
```

## What It Tests

The test script:
- ✅ Verifies ROSE installation is complete
- ✅ Detects ROSE version and features
- ✅ Tests CMake example (configure, build, run)
- ✅ Tests Autotools example (autogen, configure, build, run)
- ✅ Tests Makefile example (build, run, clean)
- ✅ Validates all programs execute successfully
- ✅ Provides colored output and detailed logging
- ✅ Creates test output directory with logs

## Example Output

```
[INFO] Found ROSE installation at: /usr/local/rose
[INFO] ROSE version: 0.11.145.339
[INFO] ROSE features: binary-analysis c/c++

========================================
Testing CMake Example
========================================
[SUCCESS] CMake configuration succeeded
[SUCCESS] CMake build succeeded
[SUCCESS] simple_analyzer executed successfully
[SUCCESS] CMake example test passed

========================================
Testing Autotools Example
========================================
[SUCCESS] Build system generated
[SUCCESS] Configuration succeeded
[SUCCESS] Build succeeded
[SUCCESS] simple_analyzer executed successfully
[SUCCESS] Autotools example test passed

========================================
Testing Makefile Example
========================================
[SUCCESS] Build succeeded
[SUCCESS] simple_analyzer executed successfully
[SUCCESS] Makefile example test passed

========================================
Test Summary
========================================
Tests Run: 3
Tests Passed: 3
Tests Failed: 0

✓ CMake example: PASSED
✓ Autotools example: PASSED
✓ Makefile example: PASSED

[SUCCESS] All tests passed!
```

## CI/CD Integration

For continuous integration pipelines:

```bash
#!/bin/bash
# CI/CD test script

# Build ROSE
cmake -B build -DCMAKE_INSTALL_PREFIX=/opt/rose
cmake --build build --target install

# Test external project examples
cd docs/examples
./test-all-examples.sh /opt/rose

# Exit code indicates success (0) or failure (non-zero)
```

## Requirements

For the test script to work:

1. **ROSE Installation**: ROSE must be fully installed (not just built)
   ```bash
   cmake --build _build --target install
   ```

2. **Same Environment**: Use the same environment that built ROSE
   ```bash
   # If ROSE was built with spock-shell
   spock-shell --with-file env ./test-all-examples.sh /path/to/rose/install
   ```

3. **Build Tools**:
   - CMake (for CMake example)
   - Autotools: autoconf, automake, libtool (for Autotools example)
   - make (for all examples)
   - pkg-config (for all examples)

4. **Compiler**: C++ compiler with C++14 support

## Test Output

The script creates `test-output/` directory with:
```
test-output/
├── cmake-test/
│   ├── build/
│   ├── cmake_configure.log
│   ├── cmake_build.log
│   └── simple_analyzer_output.log
├── autotools-test/
│   ├── autogen.log
│   ├── configure.log
│   ├── make.log
│   └── simple_analyzer_output.log
└── makefile-test/
    ├── make.log
    └── simple_analyzer_output.log
```

Review these logs if tests fail to diagnose issues.

## Exit Codes

- `0` - All tests passed
- `1` - Usage error or missing ROSE installation
- `2` - CMake example failed
- `3` - Autotools example failed
- `4` - Makefile example failed
