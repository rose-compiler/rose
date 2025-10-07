# ROSE External Project Example - Autotools

This directory contains a complete example of how to use ROSE in an external Autotools (GNU Autoconf/Automake) project.

## Overview

This example demonstrates:
- Using `PKG_CHECK_MODULES` to find ROSE via pkg-config
- Automatically obtaining ROSE's compiler and linker flags
- Building a ROSE-based tool with Autotools
- Integration with standard GNU build system conventions

## Prerequisites

1. **ROSE installed** - ROSE must be fully installed (not just built)
2. **Autotools** - autoconf, automake, and libtool must be installed
3. **pkg-config** - Used to find ROSE
4. **m4 macro for C++14** - The `AX_CXX_COMPILE_STDCXX` macro (from autoconf-archive)

### Installing Prerequisites

On Debian/Ubuntu:
```bash
sudo apt-get install autoconf automake libtool pkg-config autoconf-archive
```

On RHEL/CentOS/Fedora:
```bash
sudo yum install autoconf automake libtool pkgconfig autoconf-archive
```

## Building

### Step 1: Generate Build System

First, generate the configure script and Makefile.in files:

```bash
./autogen.sh
```

This runs `autoreconf` to create the configure script from `configure.ac` and `Makefile.in` from `Makefile.am`.

### Step 2: Set PKG_CONFIG_PATH

Tell pkg-config where to find ROSE:

```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
```

### Step 3: Configure

Run the configure script:

```bash
./configure
```

The configure script will:
- Check for a C++ compiler with C++14 support
- Search for ROSE using pkg-config
- Verify ROSE version is 0.11 or later
- Generate Makefiles

### Step 4: Build

```bash
make
```

This compiles `simple_analyzer` with the appropriate ROSE flags.

### Step 5: Run

```bash
./simple_analyzer test_input.c
```

## Complete Build Example

```bash
# Clone or extract the example
cd external-project-autotools

# Generate build system
./autogen.sh

# Set up environment
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH

# Configure, build, and run
./configure
make
./simple_analyzer test_input.c
```

## Project Structure

- **configure.ac** - Autoconf input file that defines build configuration
- **Makefile.am** - Automake input file that defines build rules
- **simple_analyzer.cpp** - Example ROSE analyzer source code
- **autogen.sh** - Helper script to generate configure script
- **test_input.c** - Sample C file for testing the analyzer

## Key Autotools Concepts

### configure.ac

The `configure.ac` file defines the build configuration:

```bash
# Check for ROSE using pkg-config
PKG_CHECK_MODULES([ROSE], [rose >= 0.11], [
    have_rose=yes
    AC_DEFINE([HAVE_ROSE], [1], [Define if ROSE is available])
], [
    have_rose=no
    AC_MSG_ERROR([ROSE not found. Please install ROSE and set PKG_CONFIG_PATH.])
])
```

This:
1. Searches for ROSE via pkg-config
2. Requires version 0.11 or later
3. Sets `ROSE_CFLAGS` and `ROSE_LIBS` variables
4. Defines `HAVE_ROSE` preprocessor macro

### Makefile.am

The `Makefile.am` file defines build rules:

```makefile
bin_PROGRAMS = simple_analyzer

simple_analyzer_SOURCES = simple_analyzer.cpp
simple_analyzer_CXXFLAGS = $(ROSE_CFLAGS) $(AM_CXXFLAGS)
simple_analyzer_LDADD = $(ROSE_LIBS)
```

This:
1. Declares `simple_analyzer` as a program to build
2. Specifies source files
3. Adds ROSE compiler flags (`ROSE_CFLAGS` from pkg-config)
4. Adds ROSE linker flags (`ROSE_LIBS` from pkg-config)

## Advanced Usage

### Optional ROSE Support

To make ROSE optional instead of required:

```bash
# In configure.ac, change PKG_CHECK_MODULES to:
PKG_CHECK_MODULES([ROSE], [rose >= 0.11], [have_rose=yes], [have_rose=no])
AM_CONDITIONAL([HAVE_ROSE], [test "x$have_rose" = "xyes"])
```

```makefile
# In Makefile.am, add conditional:
if HAVE_ROSE
bin_PROGRAMS = simple_analyzer
simple_analyzer_SOURCES = simple_analyzer.cpp
simple_analyzer_CXXFLAGS = $(ROSE_CFLAGS)
simple_analyzer_LDADD = $(ROSE_LIBS)
endif
```

### Multiple Programs

To build multiple programs with different features:

```makefile
bin_PROGRAMS = simple_analyzer another_tool

simple_analyzer_SOURCES = simple_analyzer.cpp
simple_analyzer_CXXFLAGS = $(ROSE_CFLAGS)
simple_analyzer_LDADD = $(ROSE_LIBS)

another_tool_SOURCES = another_tool.cpp
another_tool_CXXFLAGS = $(ROSE_CFLAGS)
another_tool_LDADD = $(ROSE_LIBS)
```

### Distribution

Create a source tarball for distribution:

```bash
make dist
```

This creates `rose-external-example-1.0.tar.gz` that can be distributed and built on other systems with the standard:

```bash
tar xzf rose-external-example-1.0.tar.gz
cd rose-external-example-1.0
./configure
make
make install
```

## Troubleshooting

### "configure: error: ROSE not found"

**Problem:** pkg-config cannot find ROSE.

**Solution:** Set `PKG_CONFIG_PATH`:
```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
./configure
```

### "configure: error: C++ compiler cannot create executables"

**Problem:** C++ compiler is not properly configured.

**Solution:** Ensure you have a working C++ compiler:
```bash
which g++
g++ --version
```

### "error: required file './compile' not found"

**Problem:** Autotools helper scripts are missing.

**Solution:** Run `autogen.sh` or `autoreconf --install`:
```bash
./autogen.sh
```

### "configure: error: C++ compiler does not accept C++14"

**Problem:** Your C++ compiler doesn't support C++14, or the AX_CXX_COMPILE_STDCXX macro is not available.

**Solution:** Install autoconf-archive or manually specify a newer compiler:
```bash
sudo apt-get install autoconf-archive  # Debian/Ubuntu
# Or
./configure CXX=g++-7  # Use a specific compiler
```

### Runtime library errors

**Problem:** "error while loading shared libraries: librose.so"

**Solution:** Add ROSE's library directory to `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
./simple_analyzer test_input.c
```

### "Cannot find Boost" or other dependencies

**Problem:** ROSE's dependencies are not available in your environment.

**Solution:** Build in the same environment used to build ROSE:
```bash
# If ROSE was built with spock-shell
spock-shell --with-file env bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
./configure
make
```

## Comparison with CMake

### Advantages of Autotools:
- Standard on Unix/Linux systems
- Works well with system package managers
- Robust cross-compilation support
- Widely understood by developers

### Advantages of CMake:
- More modern and easier to learn
- Better Windows support
- More powerful for complex projects
- Better IDE integration

For ROSE projects, **CMake is generally recommended** for new projects due to better integration with ROSE's exported targets. However, Autotools remains a valid choice for Unix-focused projects or when integrating with existing Autotools-based codebases.

## Next Steps

- See `../../cmake-integration.md` for comprehensive documentation
- See `../external-project-cmake/` for a CMake-based example
- See `../external-project-makefile/` for a plain Makefile example

## License

This example is provided as part of the ROSE project for educational purposes.
