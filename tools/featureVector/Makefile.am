include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#------------------------------------------------------------------------------
# Subdirectories
#------------------------------------------------------------------------------
SUBDIRS =

#------------------------------------------------------------------------------
# Global Build Flags
#------------------------------------------------------------------------------
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

#------------------------------------------------------------------------------
# ROSE Dependency
#------------------------------------------------------------------------------
# Ensure librose has been built since our binaries will link with it
librose:
	$(MAKE) librose.la -C $(top_builddir)/src

#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------
all-local: librose

check-local: $(TEST_TARGETS)

$(TEST_TARGETS):
	@$(RTH_RUN) CMD="./$(@:.test=)" $(top_srcdir)/scripts/test_exit_status $@

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot

#------------------------------------------------------------------------------
# Unit Tests
#------------------------------------------------------------------------------
# MODIFY THIS SECTION TO ADD NEW TESTS:
#
#   1. Add executable name to BIN_EXECUTABLES
#   2. Add new <EXECUTABLE>_SOURCES variable listing the executable's source
#------------------------------------------------------------------------------
BIN_EXECUTABLES = \
	featureVector

featureVector_SOURCES = featureVector.cpp

#------------------------------------------------------------------------------
bin_PROGRAMS = \
	$(BIN_EXECUTABLES)
#------------------------------------------------------------------------------

