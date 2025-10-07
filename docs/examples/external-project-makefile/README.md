# ROSE External Project Example - Plain Makefile

This directory contains a complete example of how to use ROSE in a project using plain Makefiles (without CMake or Autotools).

## Overview

This example demonstrates:
- Using `pkg-config` to obtain ROSE compiler and linker flags
- Building a ROSE-based tool with a simple Makefile
- Checking for ROSE availability at build time
- Manual dependency management

## Prerequisites

1. **ROSE installed** - ROSE must be fully installed (not just built)
2. **pkg-config** - Used to find ROSE
3. **GNU Make** - Or compatible make utility
4. **C++ compiler** - g++, clang++, or compatible with C++14 support

## Building

### Quick Start

```bash
# Set PKG_CONFIG_PATH to find ROSE
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH

# Build
make

# Run
./simple_analyzer test_input.c
```

### Detailed Steps

#### Step 1: Set PKG_CONFIG_PATH

Tell pkg-config where to find ROSE's `.pc` file:

```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
```

You can verify ROSE is found:

```bash
pkg-config --modversion rose
# Should output: 0.11.145.339 (or your ROSE version)

pkg-config --cflags rose
# Should output: -I/path/to/rose/include/rose -std=c++14 ...

pkg-config --libs rose
# Should output: -L/path/to/rose/lib -lrose ...
```

#### Step 2: Build

```bash
make
```

This will:
1. Check if ROSE is available via pkg-config
2. Get ROSE compiler flags with `pkg-config --cflags rose`
3. Get ROSE linker flags with `pkg-config --libs rose`
4. Compile `simple_analyzer.cpp`
5. Link the executable

#### Step 3: Set LD_LIBRARY_PATH (if needed)

If ROSE's library is not in a standard location:

```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
```

#### Step 4: Run

```bash
./simple_analyzer test_input.c
```

## Complete Build Example

```bash
# Navigate to the example directory
cd external-project-makefile

# Set up environment
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH

# Build
make

# Test
./simple_analyzer test_input.c

# Clean
make clean
```

## Project Structure

- **Makefile** - Build rules and dependency configuration
- **simple_analyzer.cpp** - Example ROSE analyzer source code
- **test_input.c** - Sample C file for testing the analyzer
- **README.md** - This file

## Makefile Explained

### Detecting ROSE

```makefile
ROSE_FOUND := $(shell $(PKG_CONFIG) --exists rose && echo yes || echo no)

ifeq ($(ROSE_FOUND),yes)
    ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
    ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)
else
    $(error ROSE not found. Please set PKG_CONFIG_PATH)
endif
```

This:
1. Checks if `pkg-config` can find `rose`
2. If found, gets compiler and linker flags
3. If not found, stops with an error message

### Building the Program

```makefile
simple_analyzer: simple_analyzer.o
	$(CXX) $^ $(LDFLAGS) -o $@

simple_analyzer.o: simple_analyzer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

This:
1. Compiles `simple_analyzer.cpp` to `simple_analyzer.o` with ROSE's CFLAGS
2. Links `simple_analyzer.o` to create `simple_analyzer` with ROSE's LIBS

## Customization

### Changing the Compiler

```bash
make CXX=clang++
```

### Adding Extra Flags

```bash
make CXXFLAGS="-O3 -march=native"
```

### Using a Different pkg-config

```bash
make PKG_CONFIG=/usr/local/bin/pkg-config
```

### Building Multiple Programs

Extend the Makefile:

```makefile
PROGRAMS = simple_analyzer another_tool

all: $(PROGRAMS)

another_tool: another_tool.o
	$(CXX) $^ $(LDFLAGS) -o $@

another_tool.o: another_tool.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

## Advanced Features

### Optional Dependencies

Make ROSE optional instead of required:

```makefile
ROSE_FOUND := $(shell $(PKG_CONFIG) --exists rose && echo yes || echo no)

ifeq ($(ROSE_FOUND),yes)
    PROGRAMS += rose_analyzer
    ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
    ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)
else
    $(warning ROSE not found, skipping rose_analyzer)
endif
```

### Parallel Build

Use `-j` flag for parallel compilation:

```bash
make -j4
```

### Verbose Build

See the actual commands being executed:

```bash
make V=1
```

Add to Makefile:

```makefile
ifeq ($(V),1)
    Q =
else
    Q = @
endif

simple_analyzer.o: simple_analyzer.cpp
	@echo "  CXX     $@"
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@
```

### Installation

Add install target:

```makefile
PREFIX ?= /usr/local

install: all
	install -d $(PREFIX)/bin
	install -m 0755 $(PROGRAMS) $(PREFIX)/bin
```

Usage:

```bash
make install PREFIX=/opt/mytools
```

## Troubleshooting

### "ROSE not found"

**Problem:** pkg-config cannot locate ROSE.

**Solution:** Set `PKG_CONFIG_PATH`:
```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
make
```

### "rose.pc: No such file or directory"

**Problem:** ROSE's pkg-config file is not installed.

**Solution:** Verify ROSE installation:
```bash
ls /path/to/rose/install/lib/pkgconfig/rose.pc
```

If missing, ROSE may not be fully installed. Run `make install` in your ROSE build directory.

### "error while loading shared libraries: librose.so"

**Problem:** Runtime linker cannot find ROSE's shared library.

**Solution:** Add ROSE's library directory to `LD_LIBRARY_PATH`:
```bash
export LD_LIBRARY_PATH=/path/to/rose/install/lib:$LD_LIBRARY_PATH
./simple_analyzer test_input.c
```

Or use `RPATH` at build time (add to linker flags):
```makefile
LDFLAGS += -Wl,-rpath,$(shell $(PKG_CONFIG) --variable=libdir rose)
```

### "Cannot find -lboost_system"

**Problem:** ROSE's dependencies are not in standard library paths.

**Solution:** Build in the same environment used to build ROSE:
```bash
# If ROSE was built with spock-shell
spock-shell --with-file env bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
make
```

### Compilation errors with missing headers

**Problem:** ROSE headers include optional dependency headers.

**Solution:** Ensure you're building in the same environment as ROSE was built, with access to all dependencies (Boost, Capstone, etc.).

## Comparison with Other Build Systems

### Plain Makefile vs. CMake

**Makefile Advantages:**
- Simple and direct
- No configuration step
- Easy to understand and debug
- No external build tools needed (just make)

**CMake Advantages:**
- Better dependency tracking
- Automatic parallel builds
- Cross-platform (Windows support)
- Better IDE integration
- Automatic dependency discovery

**Recommendation:** For simple ROSE projects or quick prototypes, Makefiles work well. For larger projects or cross-platform development, CMake is recommended.

### Plain Makefile vs. Autotools

**Makefile Advantages:**
- Much simpler
- No generated files
- Faster to set up
- Easier to maintain

**Autotools Advantages:**
- Cross-platform configuration
- Standard GNU conventions
- Better portability
- Package distribution support

**Recommendation:** For Unix-only projects that don't need distribution, plain Makefiles are simpler. For projects that need to be distributed and built on diverse systems, Autotools provides better portability.

## Tips for Makefile Projects

1. **Always use pkg-config** for finding libraries - don't hardcode paths
2. **Check for errors early** - detect missing dependencies during Makefile parsing
3. **Use RPATH** for non-standard library locations
4. **Provide good error messages** - help users diagnose problems
5. **Support standard variables** - CXX, CXXFLAGS, LDFLAGS, PREFIX
6. **Include a help target** - `make help` to show usage
7. **Test in clean environment** - ensure it works without your development setup

## Next Steps

- See `../../cmake-integration.md` for comprehensive ROSE integration documentation
- See `../external-project-cmake/` for a more powerful CMake-based example
- See `../external-project-autotools/` for Autotools integration

## License

This example is provided as part of the ROSE project for educational purposes.
