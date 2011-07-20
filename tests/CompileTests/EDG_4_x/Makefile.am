include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################################################
# These are C testcodes that test the new EDG Interface (using EDG 4.0) #
#########################################################################

bin_PROGRAMS = EDG_4_x_Translator

# This tests the ability to read and regnerate the input application and compile it using the backend compiler.
EDG_4_x_Translator_SOURCES = EDG_4_x_Translator.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = C-tests.inc Cxx-tests.inc

check-local:
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/NewEDGInterface_C_tests: (ROSE not configured for these tests) ******"
	@echo "****************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] test*.[cC]
	rm -rf QMTest

include C-tests.inc
include Cxx-tests.inc

check-tests: check-Ctests check-Cxxtests
