# ROSE External Project Examples

This directory contains complete working examples demonstrating how to integrate ROSE into external projects using different build systems.

## Available Examples

### 1. CMake (`external-project-cmake/`)
Modern CMake example with:
- `find_package(Rose)` integration
- Feature detection (binary analysis, Fortran, etc.)
- Multiple conditional programs
- Based on ROSE's bat-lsv.C for binary analysis

### 2. Autotools (`external-project-autotools/`)
GNU Autotools example with:
- `PKG_CHECK_MODULES` for ROSE detection
- Standard Autoconf/Automake setup
- pkg-config integration

### 3. Makefile (`external-project-makefile/`)
Plain Makefile example with:
- pkg-config for compiler/linker flags
- Simple and straightforward
- No configuration step needed

## Quick Start

Each example includes its own README with detailed instructions.

## Testing All Examples

A comprehensive test script is provided to validate all examples:

### Usage

```bash
./test-all-examples.sh /path/to/rose/install
```

### What It Tests

The test script:
- ✅ Verifies ROSE installation is complete
- ✅ Detects ROSE version and features
- ✅ Tests CMake example (configure, build, run)
- ✅ Tests Autotools example (autogen, configure, build, run)
- ✅ Tests Makefile example (build, run, clean)
- ✅ Validates all programs execute successfully
- ✅ Provides colored output and detailed logging
- ✅ Creates test output directory with logs

### Example Output

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

### CI/CD Integration

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

### Requirements

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

### Test Output

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

### Exit Codes

- `0` - All tests passed
- `1` - Usage error or missing ROSE installation
- `2` - CMake example failed
- `3` - Autotools example failed
- `4` - Makefile example failed

## Troubleshooting

### "Could not find a package configuration file provided by Rose"

Set `CMAKE_PREFIX_PATH`:
```bash
export CMAKE_PREFIX_PATH=/path/to/rose/install
```

### "Package 'rose' not found"

Set `PKG_CONFIG_PATH`:
```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
```

### "error while loading shared libraries: librose.so"

Set `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
```

### "Cannot find Boost" or other dependencies

Build in the same environment as ROSE:
```bash
# If ROSE was built with spock-shell
spock-shell --with-file env bash
export CMAKE_PREFIX_PATH=/path/to/rose/install
cd external-project-cmake
cmake -B build
```

### Autotools test skipped

Install autotools:
```bash
# Debian/Ubuntu
sudo apt-get install autoconf automake libtool autoconf-archive

# RHEL/CentOS/Fedora
sudo yum install autoconf automake libtool autoconf-archive
```

## Documentation

- **Comprehensive Guide**: See `../cmake-integration.md` for detailed documentation
- **Each Example**: Contains its own README with specific instructions
- **Integration Tests**: See `../../cmake-integration/` for additional test cases

## For ROSE Developers

When making changes to ROSE's CMake configuration:

1. **Test locally**:
   ```bash
   cmake --build _build --target install
   ./docs/examples/test-all-examples.sh $ROSE_INSTALL_PREFIX
   ```

2. **Add to CI/CD pipeline**:
   - Run after ROSE installation
   - Verify all examples build and run
   - Check exit code

3. **Update examples** when ROSE's API changes:
   - Keep examples in sync with ROSE development
   - Test examples regularly
   - Update documentation as needed

## License

These examples are provided as part of the ROSE project for educational purposes.
