# Integrating ROSE into Your GNU Autotools Project

This guide explains how to integrate ROSE into an external project
when that project uses GNU Autotools. ROSE provides modern `pkg-config`
package configuration files that make integration straightforward.

As of 2025-10-10, due to the unpopularity of GNU Autotools as a
suitable modern build system, these instructions are largely
untested. Please submit pull requests to update this document.

# Summary Overview

Here are the main things you need to do to use ROSE in your project.

## configure.ac

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

## Makefile.am

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

# Finding ROSE

If ROSE is installed in a default location like most other system
libraries, then `pkg-config` will automatically find it.

When ROSE is installed in non-default locations, which is typically
the case for ROSE developers who might have dozens of versions and
configuration installed at once, you need to tell `pkg-config` which
installation of ROSE it should use.

```bash
export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:$PKG_CONFIG_PATH
./configure
```

# Full Boilerplate Header

We don't currently provide a boilerplate header for projects using
GNU Autotools due to its declining popularity as a modern build
system.

# ROSE Feature Tests

Your project probably depends on specific ROSE features, such as C/C++
analysis and SMT solver support.  The boilerplate above will print
what features are configured, but you can also check for them and fail
if they don't meet your project's requirements.

There is no easy way to test what ROSE features are enabled since
`pkg-config` doesn't expose these yet.  You can detect what ROSE
features are enabled by compiling a C program that includes ROSE's
"featureTests.h" and checking for various C preprocessor macros.
