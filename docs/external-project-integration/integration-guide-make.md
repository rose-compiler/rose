# Integrating ROSE into your GNU Make Project

This guide explains how to integrate ROSE into an external project
when that project uses GNU Make. ROSE provides modern `pkg-config`
configuration files that make integration straightforward.

# Summary Overview

Here are the main things you need to do to use ROSE in your project.

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

# Finding ROSE

If ROSE is installed in a default location like most other system
libraries, then `pkg-config` will automatically find it.

When ROSE is installed in non-default locations, which is typically
the case for ROSE developers who might have dozens of versions and
configuration installed at once, you need to tell `pkg-config` which
installation of ROSE it should use.  You have a couple options:

```bash
# Set PKG_CONFIG_PATH before running make
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
make

# Or pass it directly
PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig make
```

Sometimes a project might depend on a particular version of ROSE and
therefore needs to check that it found a valid version.

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
```

# Full Boilerplate Header

We don't currently provide a boilerplate header for projects using
pure GNU Make due to its declining popularity as a modern build
system.

# ROSE Feature Tests

Your project probably depends on specific ROSE features, such as C/C++
analysis and SMT solver support.  The boilerplate above will print
what features are configured, but you can also check for them and fail
if they don't meet your project's requirements.

There is no easy way to test what ROSE features are enabled within the
Makefile. You can detect what ROSE features are enabled by compiling a
C program that includes ROSE's "featureTests.h" and checking for
various C preprocessor macros.
