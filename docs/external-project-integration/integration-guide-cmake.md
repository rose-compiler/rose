# Integrating ROSE into Your CMake Project

This guide explains how to integrate ROSE into an external project
when that project uses CMake. ROSE provides modern CMake package
configuration files that make integration straightforward.

**Key benefits:**
- Include directories are automatically added
- Required dependencies (Boost, etc.) are automatically found
- Compiler features (C++14) are automatically set
- No manual configuration needed

# Summary Overview

Here are the main things you need to do to use ROSE in your project.

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

# Finding ROSE

If ROSE is installed in a default location like most other system
libraries, then the `find_package` will automatically find it.

When ROSE is installed in non-default locations, which is typically
the case for ROSE developers who might have dozens of versions and
configuration installed at once, you need to tell `find_package` which
installation of ROSE it should use.  You have a few options:

```bash
# Option 1: Put ROSE in your PATH environment variable
export PATH="$ROSE_ROOT/bin:$PATH"
cmake ...

# Option 2: Set CMAKE_PREFIX_PATH when configuring
cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..

# Option 3: Set CMAKE_PREFIX_PATH environment variable
export CMAKE_PREFIX_PATH=/path/to/rose/install
cmake ..

# Option 4: Set Rose_DIR to the cmake directory
cmake -DRose_DIR=/path/to/rose/install/lib/cmake/Rose ..
```

Sometimes a project might depend on a particular version of ROSE and
therefore needs to check that it found a valid version.

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

# Full Boilerplate Header

This section describes a more robust, full implementation.

If you're starting a new project or adding CMake to a project, it
might be best to copy the CMakeLists.txt file from external project
CMake example, which you can find in
"$ROSE/docs/external-project-integration/external-project-cmake". The
first part of this file can be identical for every project. All you
need to set is the project name at the top of the file, and then
change the bottom of the file to suite your needs.

```cmake
set(PROJECT_NAME MyRoseTool)

# [snip,snip] Top half of the example CMakeLists.txt

# Insert checks for ROSE features you need.

# Insert rules for how to build your project.

```

# ROSE Feature Tests

Your project probably depends on specific ROSE features, such as C/C++
analysis and SMT solver support.  The boilerplate above will print
what features are configured, but you can also check for them and fail
if they don't meet your project's requirements.

```cmake
if(NOT Rose_ENABLE_C)
  message(FATAL_ERROR "These tools require ROSE to be configured with C/C++ analysis")
endif()

if(NOT Rose_ENABLE_BINARY_ANALYSIS)
  message(FATAL_ERROR "These tools require ROSE to be configured with binary analysis")
endif()

if(NOT Rose_Z3_FOUND)
  message(FATAL_ERROR "These tools require ROSE to be configured with the Z3 SMT solver")
endif()
```

Some available feature flags, although the Boilerplate Header has code you can
uncomment if you want to see **all** ROSE CMake variables:
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

# Best Practices for the Project

## 1. Always Use Namespaced Targets

```cmake
# Good
target_link_libraries(my_tool PRIVATE Rose::rose)

# Avoid
target_link_libraries(my_tool PRIVATE rose)
```

## 2. Use Modern CMake Commands

```cmake
# Good - target-based
target_link_libraries(my_tool PRIVATE Rose::rose)

# Avoid - directory-based
include_directories(${ROSE_INCLUDE_DIRS})
link_directories(${ROSE_LIBRARY_DIRS})
```

## 3. Specify Version Requirements

```cmake
# Good - explicit version requirement
find_package(Rose 0.11 REQUIRED)

# Less good - accept any version
find_package(Rose REQUIRED)
```

## 4. Check Required Features

```cmake
find_package(Rose REQUIRED)

if(NOT Rose_ENABLE_BINARY_ANALYSIS)
    message(FATAL_ERROR "This project requires ROSE with binary analysis support")
endif()
```

## 5. Build in the Same Environment as ROSE

ROSE has many dependencies, many of which are optional and some of
which are C++ libraries with no standard application binary interface
(ABI).  For best results, build your project in the same environment
where ROSE was built, or at least use compatible versions and
configurations of the dependent libraries.

For RMC/Spock users, ROSE installs "lib/rmc/rose" which lists some of
these dependencies, their version numbers, and their installation
hashes. Running your build commands using this configuration will
ensure that compatible libraries (at least those managed by RMC/Spock)
are used by your project. For example:

```bash
spock-shell --with-file /usr/local/lib/rmc/rose cmake ...
```

If there is a demand for this, we can also do something similar for
other environment management tools like Spack and Nix.

# Advanced Usage

## Using ROSE from the Build Tree

For development, you can use ROSE directly from its build directory without installing:

```cmake
# Point to build directory instead of install directory
set(CMAKE_PREFIX_PATH /path/to/rose/build)
find_package(Rose REQUIRED)
```

Note: Build tree usage may have limitations compared to install tree usage.

## Cross-Compiling

When cross-compiling, ensure dependencies match the target architecture:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
      -DCMAKE_PREFIX_PATH=/path/to/rose/install/arm \
      ..
```
