# ROSE CMake Integration Tests

This directory contains integration tests for the ROSE CMake modernization project. These tests verify that external projects can successfully find, configure, build, and link against an installed ROSE library using modern CMake practices.

## Purpose

These tests validate the CMake modernization work described in `docs/CMakeModernizationPlan.md`, specifically:
- CMake package configuration (`RoseConfig.cmake`)
- Namespaced targets (`Rose::rose`)
- Version checking (`RoseConfigVersion.cmake`)
- pkg-config support (`rose.pc`)
- Dependency propagation

## Test Location

These tests are located in the **main ROSE repository** (not the separate `tests/` repository) because:
1. They test the CMake build system infrastructure itself, not ROSE functionality
2. They need to run against both build tree and install tree
3. They should be part of ROSE's core CI pipeline
4. They are quick to run and don't require the full ROSE test suite

The separate `tests/` repository is for comprehensive functionality testing of ROSE tools and libraries.

## Test Structure

```
cmake-integration/
├── 01-basic-cmake/          # Basic find_package(Rose) test
├── 02-version-check/        # Version compatibility tests
├── 03-pkgconfig/            # pkg-config integration
├── run-all-tests.sh         # Master test runner
└── README.md                # This file
```

## Prerequisites

1. **Install ROSE**: These tests require a fully installed ROSE (not just built)
   ```bash
   cd _build
   cmake --build . --target install
   ```

2. **Same Environment as ROSE Build**: The tests must run in the same environment (or an environment with the same dependencies) that was used to build ROSE. This is because `RoseConfig.cmake` needs to find all of ROSE's dependencies (Boost, etc.).

   **If you built ROSE using RMC/Spock:**
   ```bash
   # Load the same spock-shell environment
   spock-shell --with-file /path/to/your/rose/build/tree/.spock
   ```

3. **Set CMAKE_PREFIX_PATH**: Point to your ROSE installation
   ```bash
   export CMAKE_PREFIX_PATH=/path/to/rose/install
   ```

4. **Set LD_LIBRARY_PATH** (if needed, but hopefully not):
   ```bash
   export LD_LIBRARY_PATH=$CMAKE_PREFIX_PATH/lib:$CMAKE_PREFIX_PATH/lib64:$LD_LIBRARY_PATH
   ```

## Running the Tests

### Important: Environment Requirements

These tests must run in an environment that has access to all of ROSE's dependencies. If you built ROSE using RMC/Spock, you must run the tests in the same spock-shell environment.

**Example with spock-shell:**
```bash
# Load the environment (same as used for building ROSE)
spock-shell --with-file /path/to/your/rose/build/tree/.spock

# Inside the spock-shell, set paths and run tests
export CMAKE_PREFIX_PATH=/path/to/rose/install
cd cmake-integration
./run-all-tests.sh
```

### Run All Tests
```bash
cd cmake-integration
export CMAKE_PREFIX_PATH=/path/to/rose/install
./run-all-tests.sh
```

### Run Individual Tests
```bash
cd cmake-integration/01-basic-cmake
export CMAKE_PREFIX_PATH=/path/to/rose/install
./test.sh

# Or test 02
cd cmake-integration/02-version-check
./test.sh

# Or test 03
cd cmake-integration/03-pkgconfig
./test.sh
```

## Test Descriptions

### Test 01: Basic CMake Integration
- **Purpose**: Verify basic `find_package(Rose)` works
- **Validates**:
  - `RoseConfig.cmake` is found and loaded via `CMAKE_PREFIX_PATH`
  - `Rose::rose` target exists
  - Basic compilation and linking succeeds
  - Feature flags are accessible
  - Works with ROSE installed to any location (relocatable)

### Test 02: Version Check
- **Purpose**: Test version compatibility checking
- **Validates**:
  - `RoseConfigVersion.cmake` works
  - `find_package(Rose VERSION)` syntax works
  - Version constraints are enforced
  - Impossible versions are rejected

### Test 03: pkg-config Integration
- **Purpose**: Test non-CMake build systems
- **Validates**:
  - `rose.pc` is generated and installed
  - `pkg-config --cflags rose` returns correct flags
  - `pkg-config --libs rose` returns correct libraries
  - Makefile-based builds work

## Expected Output

When all tests pass, you should see:
```
========================================
All tests passed!
========================================
Total: 3 tests
Passed: 3
Failed: 0
```

## Troubleshooting

### "Could NOT find Boost (missing: ...)" or other dependency errors
This is the **most common error** and means the test environment doesn't have access to ROSE's dependencies.

**Solution**: Run the tests in the same environment used to build ROSE:
```bash
# If you built ROSE with spock-shell
spock-shell --with-file /path/to/your/dependencies.yaml

# Inside spock-shell, run the tests
export CMAKE_PREFIX_PATH=/path/to/rose/install
cd cmake-integration
./run-all-tests.sh
```

The test environment needs access to:
- Boost (same version ROSE was built with)
- All PUBLIC dependencies: Capstone, Gcrypt, GpgError, Dlib (if ROSE was built with them)

### "Rose_DIR not found"
- Ensure ROSE is fully installed (not just built)
- Check that `CMAKE_PREFIX_PATH` points to the installation directory
- Verify `$CMAKE_PREFIX_PATH/lib/cmake/Rose/RoseConfig.cmake` exists

### "rose.pc not found"
- Ensure ROSE was built with pkg-config support
- Check that `$CMAKE_PREFIX_PATH/lib/pkgconfig/rose.pc` exists
- Set `PKG_CONFIG_PATH` to include the pkgconfig directory

### Linking errors
- Set `LD_LIBRARY_PATH` to include ROSE's lib directory
- Verify all ROSE dependencies are accessible
- Make sure you're in the same environment as the ROSE build

### Version mismatch
- These tests are designed for ROSE 0.11.145+
- Earlier versions may not have the modernized CMake configuration

## Integration with CI

These tests should be run as part of ROSE's continuous integration:

```bash
# In CI pipeline after build
cmake --build . --target install
export CMAKE_PREFIX_PATH=$INSTALL_PREFIX
cd cmake-integration
./run-all-tests.sh
```

## See Also

- `docs/CMakeModernizationPlan.md` - Full modernization plan
- `cmake/RoseConfig.cmake.in` - Package configuration template
- `cmake/rose.pc.in` - pkg-config template
