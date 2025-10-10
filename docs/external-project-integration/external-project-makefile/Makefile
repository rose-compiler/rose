# Makefile for ROSE External Project Example
#
# This demonstrates how to use ROSE in a project with plain Makefiles
# using pkg-config to obtain compiler and linker flags.

# Configuration
PKG_CONFIG ?= pkg-config
CXX ?= g++
CXXFLAGS ?= -Wall -g
LDFLAGS ?=

# Check if ROSE is available
ROSE_FOUND := $(shell $(PKG_CONFIG) --exists rose && echo yes || echo no)

ifeq ($(ROSE_FOUND),yes)
    # Get ROSE version and flags from pkg-config
    ROSE_VERSION := $(shell $(PKG_CONFIG) --modversion rose)
    ROSE_CFLAGS := $(shell $(PKG_CONFIG) --cflags rose)
    ROSE_LIBS := $(shell $(PKG_CONFIG) --libs rose)

    # Add ROSE flags to build
    CXXFLAGS += $(ROSE_CFLAGS)
    LDFLAGS += $(ROSE_LIBS)

    # Programs to build
    PROGRAMS = simple_analyzer

    $(info Found ROSE version $(ROSE_VERSION))
    $(info ROSE_CFLAGS: $(ROSE_CFLAGS))
    $(info ROSE_LIBS: $(ROSE_LIBS))
else
    $(error ROSE not found. Please set PKG_CONFIG_PATH to point to rose.pc)
endif

# Build targets
.PHONY: all clean help

all: $(PROGRAMS)

simple_analyzer: simple_analyzer.o
	$(CXX) $^ $(LDFLAGS) -o $@

simple_analyzer.o: simple_analyzer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAMS) *.o

help:
	@echo "ROSE External Project Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all programs (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  1. Set PKG_CONFIG_PATH:"
	@echo "     export PKG_CONFIG_PATH=/path/to/rose/install/lib/pkgconfig:\$$PKG_CONFIG_PATH"
	@echo ""
	@echo "  2. Build:"
	@echo "     make"
	@echo ""
	@echo "  3. Run:"
	@echo "     ./simple_analyzer test_input.c"
	@echo ""
	@echo "Environment Variables:"
	@echo "  PKG_CONFIG   - Path to pkg-config (default: pkg-config)"
	@echo "  CXX          - C++ compiler (default: g++)"
	@echo "  CXXFLAGS     - Additional C++ compiler flags"
	@echo "  LDFLAGS      - Additional linker flags"
