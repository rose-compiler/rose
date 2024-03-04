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
	LocalRWSetGenerator	\
	Name2Node		\
	TestabilityGrader

LocalRWSetGenerator_SOURCES = LocalRWSetGeneratorMain.C
Name2Node_SOURCES = Name2NodeMain.C
TestabilityGrader_SOURCES = TestabilityGraderMain.C

#------------------------------------------------------------------------------
bin_PROGRAMS = \
	$(BIN_EXECUTABLES)
#------------------------------------------------------------------------------

