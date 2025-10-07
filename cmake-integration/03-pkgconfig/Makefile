# Makefile to test pkg-config integration for ROSE
# This tests that non-CMake build systems can use ROSE via pkg-config

PKG_CONFIG ?= pkg-config

# Query pkg-config for ROSE
ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)

# Compiler
CXX ?= g++
CXXFLAGS ?= -Wall -O2

# Targets
all: test_pkgconfig

test_pkgconfig: test_pkgconfig.cpp
	@echo "Compiling with pkg-config flags..."
	@echo "ROSE_CFLAGS: $(ROSE_CFLAGS)"
	@echo "ROSE_LIBS: $(ROSE_LIBS)"
	$(CXX) $(CXXFLAGS) $(ROSE_CFLAGS) $< $(ROSE_LIBS) -o $@

clean:
	rm -f test_pkgconfig

.PHONY: all clean
