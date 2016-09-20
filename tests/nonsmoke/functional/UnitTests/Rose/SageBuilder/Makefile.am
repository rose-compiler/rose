include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#------------------------------------------------------------------------------
# Subdirectories
#------------------------------------------------------------------------------
SUBDIRS =

#------------------------------------------------------------------------------
# Global Build Flags
#------------------------------------------------------------------------------
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/UnitTests/include/
LDADD       = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(top_builddir)/tests/UnitTests/libcommon.la

#------------------------------------------------------------------------------
# ROSE Dependency
#------------------------------------------------------------------------------
# Ensure librose has been built since our tests will link with it
librose:
	$(MAKE) librose.la -C $(top_builddir)/src

$(top_builddir)/tests/UnitTests/libcommon.la:
	$(MAKE) libcommon.la -C $(top_builddir)/tests/UnitTests

#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------
all-local: librose

check-local: $(TEST_TARGETS)

$(TEST_TARGETS):
	@$(RTH_RUN) CMD="./$(@:.test=)" $(top_srcdir)/scripts/test_exit_status $@

clean-local:
	rm -f $(TEST_TARGETS)

#------------------------------------------------------------------------------
# Unit Tests
#------------------------------------------------------------------------------
# MODIFY THIS SECTION TO ADD NEW TESTS:
#
#   1. Add executable name to PASSING_TEST_EXECUTABLES
#   2. Add new <EXECUTABLE>_SOURCES variable listing the executable's source
#------------------------------------------------------------------------------
PASSING_TEST_EXECUTABLES = \
	CaseSensitivtyUnitTests \
	SgExpressionBuildersUnitTests \
	MatlabNodeBuildersUnitTests \
	SgTypeBuildersUnitTests \
	SgStatementBuildersUnitTests \
	SupportNodesUnitTests \
	UntypedNodeBuildersUnitTests

CaseSensitivtyUnitTests_SOURCES = CaseSensitivtyUnitTests.C
SgExpressionBuildersUnitTests_SOURCES = SgExpressionBuildersUnitTests.C
MatlabNodeBuildersUnitTests_SOURCES = MatlabNodeBuildersUnitTests.C
SgTypeBuildersUnitTests_SOURCES = SgTypeBuildersUnitTests.C
SgStatementBuildersUnitTests_SOURCES = SgStatementBuildersUnitTests.C
SupportNodesUnitTests_SOURCES = SupportNodesUnitTests.C
UntypedNodeBuildersUnitTests_SOURCES = UntypedNodeBuildersUnitTests.C

#------------------------------------------------------------------------------
TEST_TARGETS = ${PASSING_TEST_EXECUTABLES:=.test}

check_PROGRAMS = \
	$(PASSING_TEST_EXECUTABLES)
#------------------------------------------------------------------------------

