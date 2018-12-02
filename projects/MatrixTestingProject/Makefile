################################################################################
# Variables
################################################################################
include rose.cfg

CC            = $(ROSE_CC)
CXX           = $(ROSE_CXX)
CPPFLAGS      = $(ROSE_CPPFLAGS) -I$(ROSE_PREFIX)/include
CFLAGS        = $(ROSE_CFLAGS)
CXXFLAGS      = $(ROSE_CXXFLAGS)
LDFLAGS       = $(ROSE_LINK_RPATHS) $(ROSE_LDFLAGS) $(ROSE_RPATHS)
ROSE_LIBTOOL  = $(ROSE_PREFIX)/bin/libtool

# Handle broken ROSE header files that don't use "#include <rose/...>"
CPPFLAGS += -I$(ROSE_PREFIX)/include/rose

COMPILE = $(ROSE_LIBTOOL) --mode=compile --tag=CXX  $(CXX)
LINK    = $(ROSE_LIBTOOL) --mode=link    --tag=CXX  $(CXX)

tools = disassemble
all: $(tools)

################################################################################
# Rules to build the main artifacts
################################################################################

# Libtool is used for compiling and linking this project, providing platform independence.
# This requires the use of libtool's naming convention for object files (*.lo).
$(addsuffix .lo, $(tools)): %.lo: %.C
	$(COMPILE) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(tools): %: %.lo
	$(LINK) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

################################################################################
# Scripts for testing the project
################################################################################

.PHONY: check
check: disassemble
	./disassemble binary_specimen

################################################################################
# Scripts to install the software
################################################################################

# Install this project's user-level artifacts into the ROSE prefix
.PHONY: install
install: $(tools)
	cp $(tools) $(ROSE_PREFIX)/bin

################################################################################
# Prerequisites
################################################################################
rose.cfg:
	@echo "error: you must copy rose-installed-make.cfg or rose-config.cfg to rose.cfg first" >&2
	@echo "       The rose.cfg file defines ROSE features necessary to compile these tools." >&2
	@false

