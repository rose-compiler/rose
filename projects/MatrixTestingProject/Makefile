################################################################################
# Variables
################################################################################
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
	./test-rose --help |cat; exit ${PIPESTATUS[0]}

################################################################################
# Scripts to install the software
################################################################################

# Install this project's user-level artifacts into the ROSE prefix
.PHONY: install
install: test-rose
	cp $^ $(ROSE_PREFIX)/bin

################################################################################
# Prerequisites
################################################################################
rose.cfg:
	@echo "error: you must copy rose-installed-make.cfg or rose-config.cfg to rose.cfg first" >&2
	@echo "       The rose.cfg file defines ROSE features necessary to compile these tools." >&2
	@false

