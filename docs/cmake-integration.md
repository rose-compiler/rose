# Integrating ROSE into Your Project

This guide explains how to use ROSE in external projects using modern CMake, Autotools, or plain Makefiles.

## Table of Contents

- [Quick Start](#quick-start)
- [CMake Integration](#cmake-integration)
- [Autotools Integration](#autotools-integration)
- [Makefile Integration](#makefile-integration)
- [Feature Detection](#feature-detection)
- [Troubleshooting](#troubleshooting)
- [Migration from Old Methods](#migration-from-old-methods)

## Quick Start

### CMake (Modern Approach - Recommended)

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyRoseTool)

# Find ROSE
find_package(Rose REQUIRED)

# Create your executable
add_executable(my_tool main.cpp)

# Link against ROSE - dependencies are automatically propagated!
target_link_libraries(my_tool PRIVATE Rose::rose)
```

### Autotools

```bash
# In configure.ac
PKG_CHECK_MODULES([ROSE], [rose >= 0.11])

# In Makefile.am
my_tool_CXXFLAGS = $(ROSE_CFLAGS)
my_tool_LDADD = $(ROSE_LIBS)
```

### Makefile

```make
ROSE_CFLAGS := $(shell pkg-config --cflags rose)
ROSE_LIBS := $(shell pkg-config --libs rose)

my_tool: main.cpp
	$(CXX) $(CXXFLAGS) $(ROSE_CFLAGS) $< $(ROSE_LIBS) -o $@
```

## CMake Integration

### Basic Usage

ROSE provides modern CMake package configuration files that make integration straightforward:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyRoseTool CXX)

# Find ROSE - CMake will automatically locate it if CMAKE_PREFIX_PATH is set
find_package(Rose REQUIRED)

# Create your executable or library
add_executable(my_analyzer analyzer.cpp)

# Link against Rose::rose - all dependencies propagate automatically
target_link_libraries(my_analyzer PRIVATE Rose::rose)
```

**Key benefits:**
- Include directories are automatically added
- Required dependencies (Boost, etc.) are automatically found
- Compiler features (C++14) are automatically set
- No manual configuration needed

### Specifying Installation Location

If ROSE is installed in a non-standard location, tell CMake where to find it:

```bash
# Option 1: Set CMAKE_PREFIX_PATH when configuring
cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..

# Option 2: Set CMAKE_PREFIX_PATH environment variable
export CMAKE_PREFIX_PATH=/path/to/rose/install
cmake ..

# Option 3: Set Rose_DIR to the cmake directory
cmake -DRose_DIR=/path/to/rose/install/lib/cmake/Rose ..
```

### Version Requirements

Request specific ROSE versions:

```cmake
# Require at least version 0.11
find_package(Rose 0.11 REQUIRED)

# Require exact version
find_package(Rose 0.11.145 EXACT REQUIRED)

# Make ROSE optional
find_package(Rose 0.11)
if(Rose_FOUND)
    message(STATUS "Found ROSE ${Rose_VERSION}")
    target_link_libraries(my_tool PRIVATE Rose::rose)
endif()
```

### Checking Available Features

Query ROSE's build-time configuration:

```cmake
find_package(Rose REQUIRED)

# Check which features are available
if(Rose_ENABLE_BINARY_ANALYSIS)
    message(STATUS "ROSE has binary analysis support")
    target_compile_definitions(my_tool PRIVATE HAVE_ROSE_BINARY_ANALYSIS)
endif()

if(Rose_ENABLE_C)
    message(STATUS "ROSE has C/C++ frontend support")
endif()

if(Rose_ENABLE_FORTRAN)
    message(STATUS "ROSE has Fortran frontend support")
endif()
```

Available feature flags:
- `Rose_ENABLE_BINARY_ANALYSIS` - Binary analysis capabilities
- `Rose_ENABLE_C` - C/C++ frontend (EDG or Clang)
- `Rose_ENABLE_CUDA` - CUDA support
- `Rose_ENABLE_JAVA` - Java frontend
- `Rose_ENABLE_OPENCL` - OpenCL support
- `Rose_ENABLE_FORTRAN` - Fortran frontend
- `Rose_ENABLE_PHP` - PHP frontend
- `Rose_ENABLE_PYTHON` - Python frontend
- `Rose_ENABLE_ADA` - Ada frontend
- `Rose_ENABLE_JOVIAL` - Jovial frontend
- `Rose_ENABLE_ASSEMBLY_SEMANTICS` - Assembly semantics
- `Rose_ENABLE_CLANG_FRONTEND` - Clang-based C/C++ frontend (vs EDG)

### Complete Example

See `docs/examples/external-project-cmake/` for a complete working example.

## Autotools Integration

ROSE provides a pkg-config file (`rose.pc`) for non-CMake build systems.

### configure.ac

```bash
# Check for ROSE with minimum version
PKG_CHECK_MODULES([ROSE], [rose >= 0.11], [
    AC_DEFINE([HAVE_ROSE], [1], [Define if ROSE is available])
], [
    AC_MSG_ERROR([ROSE not found. Please install ROSE or set PKG_CONFIG_PATH.])
])

# Make ROSE optional
PKG_CHECK_MODULES([ROSE], [rose >= 0.11], [have_rose=yes], [have_rose=no])
AM_CONDITIONAL([HAVE_ROSE], [test "x$have_rose" = "xyes"])
```

### Makefile.am

```makefile
# Add ROSE flags to your program
bin_PROGRAMS = my_analyzer

my_analyzer_SOURCES = analyzer.cpp
my_analyzer_CXXFLAGS = $(ROSE_CFLAGS) $(AM_CXXFLAGS)
my_analyzer_LDADD = $(ROSE_LIBS)

# For optional ROSE support
if HAVE_ROSE
bin_PROGRAMS += rose_analyzer
rose_analyzer_SOURCES = rose_analyzer.cpp
rose_analyzer_CXXFLAGS = $(ROSE_CFLAGS) $(AM_CXXFLAGS)
rose_analyzer_LDADD = $(ROSE_LIBS)
endif
```

### Setting PKG_CONFIG_PATH

If ROSE is installed in a non-standard location:

```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
./configure
```

### Complete Example

See `docs/examples/external-project-autotools/` for a complete working example.

## Makefile Integration

For projects using plain Makefiles, use pkg-config:

### Basic Makefile

```makefile
# Get ROSE compiler and linker flags
PKG_CONFIG ?= pkg-config
ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)

# Compiler settings
CXX ?= g++
CXXFLAGS += $(ROSE_CFLAGS)
LDFLAGS += $(ROSE_LIBS)

# Build your program
my_analyzer: analyzer.o
	$(CXX) $^ $(LDFLAGS) -o $@

analyzer.o: analyzer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o my_analyzer
```

### Checking ROSE Availability

```makefile
# Check if ROSE is available
ROSE_FOUND := $(shell $(PKG_CONFIG) --exists rose && echo yes || echo no)

ifeq ($(ROSE_FOUND),yes)
    ROSE_VERSION := $(shell $(PKG_CONFIG) --modversion rose)
    $(info Found ROSE $(ROSE_VERSION))
    ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
    ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)
    CXXFLAGS += $(ROSE_CFLAGS) -DHAVE_ROSE
    LDFLAGS += $(ROSE_LIBS)
    PROGRAMS += rose_analyzer
else
    $(warning ROSE not found, skipping ROSE-based tools)
endif

all: $(PROGRAMS)
```

### Setting PKG_CONFIG_PATH

```bash
# Set PKG_CONFIG_PATH before running make
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
make

# Or pass it directly
PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig make
```

### Complete Example

See `docs/examples/external-project-makefile/` for a complete working example.

## Feature Detection

### In CMake

```cmake
find_package(Rose REQUIRED)

# Check feature flags
if(Rose_ENABLE_BINARY_ANALYSIS)
    target_sources(my_tool PRIVATE binary_analysis.cpp)
    target_compile_definitions(my_tool PRIVATE HAVE_ROSE_BINARY_ANALYSIS)
endif()

if(Rose_ENABLE_FORTRAN)
    target_sources(my_tool PRIVATE fortran_support.cpp)
    target_compile_definitions(my_tool PRIVATE HAVE_ROSE_FORTRAN)
endif()
```

### In Autotools

You can query feature flags using the `rose-config` utility:

```bash
# In configure.ac
AC_PATH_PROG([ROSE_CONFIG], [rose-config])
if test "x$ROSE_CONFIG" != "x"; then
    # Query features (this is a manual approach)
    # The pkg-config file doesn't expose feature flags yet
    ROSE_PREFIX=$($ROSE_CONFIG --prefix)
fi
```

### In Makefiles

Feature detection is available through environment inspection or by including ROSE's configuration:

```makefile
# Query ROSE installation prefix
ROSE_PREFIX := $(shell $(PKG_CONFIG) --variable=prefix rose)

# Check for binary analysis support by testing for header
HAVE_BINARY_ANALYSIS := $(shell test -f $(ROSE_PREFIX)/include/rose/Rose/BinaryAnalysis.h && echo yes || echo no)
```

## Troubleshooting

### "Could not find a package configuration file provided by Rose"

**Problem:** CMake cannot locate ROSE's configuration files.

**Solutions:**
1. Set `CMAKE_PREFIX_PATH` to ROSE installation:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..
   ```

2. Set `Rose_DIR` directly:
   ```bash
   cmake -DRose_DIR=/path/to/rose/install/lib/cmake/Rose ..
   ```

3. Set `CMAKE_PREFIX_PATH` environment variable:
   ```bash
   export CMAKE_PREFIX_PATH=/path/to/rose/install
   cmake ..
   ```

### "Package 'rose' not found"

**Problem:** pkg-config cannot find ROSE's `.pc` file.

**Solution:** Set `PKG_CONFIG_PATH`:
```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
```

### "Could not find Boost" (or other dependencies)

**Problem:** CMake cannot find ROSE's dependencies.

**Solution:** ROSE requires the same environment that was used to build it. If you built ROSE using a tool like `spock-shell`, you need to run your project's CMake in the same environment:

```bash
# If ROSE was built with spock-shell
spock-shell --with-file env cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..
spock-shell --with-file env cmake --build .
```

### Missing Headers for Optional Dependencies

**Problem:** Compilation fails with missing headers like `<capstone/arm64.h>` or `<gcrypt.h>`.

**Explanation:** Some ROSE headers conditionally include optional dependency headers. If ROSE was built with these features, your project needs access to the same dependencies.

**Solution:** Ensure you're building in the same environment as ROSE, or ensure the dependencies are available:
- Capstone (if `Rose_CAPSTONE_FOUND` is TRUE)
- Gcrypt/GpgError (if `Rose_GCRYPT_FOUND` is TRUE)
- YAML-CPP (if `Rose_YAMLCPP_FOUND` is TRUE)
- Dlib (if `Rose_DLIB_FOUND` is TRUE)

### Runtime Library Not Found

**Problem:** Program builds but fails at runtime with "error while loading shared libraries: librose.so".

**Solution:** Add ROSE's library directory to `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
./my_analyzer
```

Or use `RPATH` in CMake:
```cmake
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
```

## Migration from Old Methods

### From FindRose.cmake (Module Mode)

**Old approach:**
```cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /path/to/rose/cmake)
find_package(Rose REQUIRED)
include_directories(${ROSE_INCLUDE_DIRS})
target_link_libraries(my_tool ${ROSE_LIBRARIES})
```

**New approach:**
```cmake
find_package(Rose REQUIRED)
target_link_libraries(my_tool PRIVATE Rose::rose)
```

**Changes:**
- No need to manually set `CMAKE_MODULE_PATH`
- Use `CMAKE_PREFIX_PATH` instead to point to installation
- No need to manually add include directories
- Use namespaced target `Rose::rose` instead of `${ROSE_LIBRARIES}`
- Dependencies are automatically found and propagated

### From rose-config Script

**Old approach:**
```bash
ROSE_CFLAGS=$(rose-config --cflags)
ROSE_LIBS=$(rose-config --libs)
g++ myapp.cpp $ROSE_CFLAGS $ROSE_LIBS -o myapp
```

**New approach:**
```bash
ROSE_CFLAGS=$(pkg-config --cflags rose)
ROSE_LIBS=$(pkg-config --libs rose)
g++ myapp.cpp $ROSE_CFLAGS $ROSE_LIBS -o myapp
```

**Changes:**
- Use `pkg-config` instead of `rose-config`
- More standardized, works with Autotools
- Better dependency tracking

### From Manual Configuration

**Old approach:**
```cmake
include_directories(/path/to/rose/include/rose)
link_directories(/path/to/rose/lib)
target_link_libraries(my_tool rose boost_system boost_filesystem ...)
```

**New approach:**
```cmake
find_package(Rose REQUIRED)
target_link_libraries(my_tool PRIVATE Rose::rose)
```

**Changes:**
- No manual include or link directories
- No need to enumerate dependencies
- Everything is automatic and relocatable

## Best Practices

### 1. Always Use Namespaced Targets

```cmake
# Good
target_link_libraries(my_tool PRIVATE Rose::rose)

# Avoid
target_link_libraries(my_tool PRIVATE rose)
```

### 2. Use Modern CMake Commands

```cmake
# Good - target-based
target_link_libraries(my_tool PRIVATE Rose::rose)

# Avoid - directory-based
include_directories(${ROSE_INCLUDE_DIRS})
link_directories(${ROSE_LIBRARY_DIRS})
```

### 3. Specify Version Requirements

```cmake
# Good - explicit version requirement
find_package(Rose 0.11 REQUIRED)

# Less good - accept any version
find_package(Rose REQUIRED)
```

### 4. Check Required Features

```cmake
find_package(Rose REQUIRED)

if(NOT Rose_ENABLE_BINARY_ANALYSIS)
    message(FATAL_ERROR "This project requires ROSE with binary analysis support")
endif()
```

### 5. Build in the Same Environment as ROSE

ROSE has many dependencies. For best results, build your project in the same environment where ROSE was built:

```bash
# If ROSE was built with spock-shell
spock-shell --with-file env cmake ..
spock-shell --with-file env cmake --build .
```

## Advanced Usage

### Using ROSE from the Build Tree

For development, you can use ROSE directly from its build directory without installing:

```cmake
# Point to build directory instead of install directory
set(CMAKE_PREFIX_PATH /path/to/rose/build)
find_package(Rose REQUIRED)
```

Note: Build tree usage may have limitations compared to install tree usage.

### Multiple ROSE Versions

If you have multiple ROSE versions installed, specify which one to use:

```bash
# Use specific version by setting CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH=/path/to/rose-0.11 ..

# Or use Rose_DIR
cmake -DRose_DIR=/path/to/rose-0.11/lib/cmake/Rose ..
```

### Cross-Compilation

When cross-compiling, ensure dependencies match the target architecture:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
      -DCMAKE_PREFIX_PATH=/path/to/rose/install/arm \
      ..
```

## Getting Help

- **Documentation:** https://github.com/rose-compiler/rose/tree/develop/docs
- **Integration Tests:** See `cmake-integration/` directory for working examples
- **Issues:** Report problems at https://github.com/rose-compiler/rose/issues

## Summary

### CMake Users
```cmake
find_package(Rose REQUIRED)
target_link_libraries(my_tool PRIVATE Rose::rose)
```

### Autotools Users
```bash
# configure.ac
PKG_CHECK_MODULES([ROSE], [rose >= 0.11])

# Makefile.am
my_tool_CXXFLAGS = $(ROSE_CFLAGS)
my_tool_LDADD = $(ROSE_LIBS)
```

### Makefile Users
```make
ROSE_CFLAGS := $(shell pkg-config --cflags rose)
ROSE_LIBS := $(shell pkg-config --libs rose)
my_tool: main.cpp
	$(CXX) $(CXXFLAGS) $(ROSE_CFLAGS) $< $(ROSE_LIBS) -o $@
```

All three approaches benefit from:
- Automatic dependency handling
- Relocatable installations
- Version compatibility checking
- Feature detection
