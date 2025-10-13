# ROSE External Project Example - CMake

This directory contains a complete example of how to use ROSE in an external CMake project.

## Overview

This example demonstrates:
- Finding ROSE using `find_package(Rose)`
- Linking against the `Rose::rose` target
- Detecting ROSE's version and features
- Building conditionally based on available features
- Creating multiple tools with different ROSE capabilities

## Building

### Prerequisites

1. ROSE must be installed (not just built)
2. Set `CMAKE_PREFIX_PATH` to point to your ROSE installation

### Build Instructions

```bash
# Configure the project
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/rose/install

# Build
cmake --build build

# Run the examples
./build/simple_analyzer
./build/binary_analyzer test_input.exe    # Only if ROSE has binary analysis
./build/fortran_analyzer test_input.f90   # Only if ROSE has Fortran support
```

### Alternative Configuration Methods

If you don't want to use `CMAKE_PREFIX_PATH`, you can use:

```bash
# Set Rose_DIR directly to the CMake config directory
cmake -B build -DRose_DIR=/path/to/rose/install/lib/cmake/Rose

# Or set CMAKE_PREFIX_PATH as an environment variable
export CMAKE_PREFIX_PATH=/path/to/rose/install
cmake -B build
```

## Example Programs

### simple_analyzer.cpp

A basic ROSE analyzer that does almost nothing and can be run no
matter what analysis features were configured when ROSE was
installed. It even works if ROSE has no analysis features enabled.

Usage:
```bash
./build/simple_analyzer
```

### binary_analyzer.cpp

A binary analysis tool that:
- Parses executable files
- Creates a binary partitioner
- Analyzes and synthesizes information about global variables
- Demonstrates ROSE's ability to recover high-level information from binaries

Based on `tools/BinaryAnalysis/bat-lsv.C` from the ROSE repository.

This example is only built if ROSE was configured with `ENABLE_BINARY_ANALYSIS=yes`.

Usage:
```bash
./build/binary_analyzer /bin/ls
./build/binary_analyzer /usr/bin/gcc
```

### fortran_analyzer.cpp

A Fortran analysis tool that:
- Parses Fortran source files
- Lists program units (PROGRAM, MODULE, SUBROUTINE, FUNCTION)

This example is only built if ROSE was configured with `ENABLE_FORTRAN=yes`.

Usage:
```bash
cat > test_input.f90 << 'EOF'
program hello
    implicit none
    print *, "Hello from Fortran!"
    call greet("ROSE")
contains
    subroutine greet(name)
        character(len=*), intent(in) :: name
        print *, "Hello, ", name
    end subroutine greet
end program hello
EOF

./build/fortran_analyzer test_input.f90
```

## Key CMake Concepts

### Finding ROSE

```cmake
# Request specific version
find_package(Rose 0.11 REQUIRED)

# Access version information
message(STATUS "Found ROSE ${Rose_VERSION}")
```

### Using ROSE

```cmake
# Create your executable
add_executable(my_tool my_tool.cpp)

# Link against Rose::rose - everything is automatic
target_link_libraries(my_tool PRIVATE Rose::rose)
```

### Feature Detection

```cmake
# Check if ROSE has binary analysis
if(Rose_ENABLE_BINARY_ANALYSIS)
    add_executable(binary_tool binary_tool.cpp)
    target_link_libraries(binary_tool PRIVATE Rose::rose)
    target_compile_definitions(binary_tool PRIVATE HAVE_ROSE_BINARY_ANALYSIS)
endif()
```

### Available Feature Flags

- `Rose_ENABLE_BINARY_ANALYSIS` - Binary analysis capabilities
- `Rose_ENABLE_C` - C/C++ frontend
- `Rose_ENABLE_FORTRAN` - Fortran frontend
- `Rose_ENABLE_JAVA` - Java frontend
- `Rose_ENABLE_PYTHON` - Python frontend
- `Rose_ENABLE_CUDA` - CUDA support
- `Rose_ENABLE_OPENCL` - OpenCL support
- `Rose_ENABLE_PHP` - PHP frontend
- `Rose_ENABLE_ADA` - Ada frontend
- `Rose_ENABLE_JOVIAL` - Jovial frontend
- `Rose_ENABLE_ASSEMBLY_SEMANTICS` - Assembly semantics
- `Rose_ENABLE_CLANG_FRONTEND` - Clang-based C/C++ frontend

## Troubleshooting

### "Could not find a package configuration file provided by Rose"

Set `CMAKE_PREFIX_PATH` to your ROSE installation:
```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/rose/install
```

### "Could not find Boost" or other dependencies

ROSE requires the same environment used to build it. If ROSE was built using a tool like `spock-shell`, you need to use the same environment:

```bash
spock-shell --with-file env cmake -B build -DCMAKE_PREFIX_PATH=/path/to/rose/install
spock-shell --with-file env cmake --build build
```

### Missing headers like <capstone/arm64.h>

Some ROSE headers conditionally include optional dependency headers. Make sure you're building in the same environment as ROSE, with access to all its dependencies.

### Runtime library errors

Add ROSE's library directory to `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
./build/simple_analyzer test_input.c
```

## Next Steps

- See `../../cmake-integration.md` for comprehensive documentation
- See `../../cmake-integration/` for integration test examples
- Explore the ROSE API documentation

## License

This example is provided as part of the ROSE project for educational purposes.
