################################################################################
# Variables
################################################################################

# Copied here by the configure script
include rose.cfg

CC            = $(ROSE_CC)
CXX           = $(ROSE_CXX)
CPPFLAGS      = $(ROSE_CPPFLAGS) -I$(ROSE_PREFIX)/include
CFLAGS        = $(ROSE_CFLAGS)
CXXFLAGS      = $(ROSE_CXXFLAGS)
LDFLAGS       = $(ROSE_LDFLAGS)
ROSE_LIBTOOL  = $(ROSE_PREFIX)/bin/libtool

# Handle broken ROSE header files that don't use "#include <rose/...>"
CPPFLAGS += -I$(ROSE_PREFIX)/include/rose

################################################################################
# Rules to build the main artifacts
################################################################################

test-rose: test-rose.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

################################################################################
# Scripts for testing the project
################################################################################

.PHONY: check
check: test-rose
	./test-rose --version
	./test-rose --version-long
	./test-rose --self-test
	./test-rose --help |cat; exit ${PIPESTATUS[0]}

################################################################################
# Scripts to install the software
################################################################################

# Install this project's user-level artifacts into the ROSE prefix
.PHONY: install
install: check
	cp test-rose $(ROSE_PREFIX)/bin

################################################################################
# Prerequisites
################################################################################
rose.cfg:
	@echo "error: you must run configure first" >&2
	@false

