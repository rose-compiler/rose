# Import definitions common to LiaOutliner submodules.
#include ../Make.inc
SUBDIRS = fortranTests

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#place to install binary, header, and libraries
#liaOutlinerbindir = $(bindir)/liaOutliner
#liaOutlinerincdir = $(includedir)/liaOutliner
#liaOutlinerlibdir = $(libdir)

# Default header file search path
INCLUDES = $(ROSE_INCLUDES)

# DQ (!/24/2008): We should not need this!
#        -I$(top_srcdir) -I$(top_builddir)
#       -I$(top_srcdir)/src/midend/astOutlining/src

# Default link flags for executables
LIAOUTLINER_LIBS = $(ROSE_LIBS)

# DQ (!/24/2008): This library is no longer required (everything is in librose.la)
# $(top_builddir)/src/midend/astOutlining/src/libliaOutliner.la

# Useful definitions for testing
#   TEST_DIR = Location of ROSE's basic C++ tests.
#   TEST_INCLUDES = Find header files upon which the test files depend.
TEST_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
TEST_INCLUDES = \
        -I$(TEST_DIR) \
        -I$(top_srcdir)/tests/CompileTests/A++Code
# end of ../Make.in----

# INCLUDES += -I.

# liaOutlinerbin_PROGRAMS =
noinst_PROGRAMS = \
        liaoutline \
        injectOutlinePragmas \
        outlineRandom \
        outlineSelection

liaoutline_SOURCES = liaoutline.cc
liaoutline_LDFLAGS = $(LIAOUTLINER_LIBS)

injectOutlinePragmas_SOURCES = \
        RandomStmtSelector.cc \
        injectOutlinePragmas.cc
injectOutlinePragmas_LDFLAGS = $(LIAOUTLINER_LIBS)

outlineRandom_SOURCES = \
        RandomStmtSelector.cc \
        outlineRandom.cc
outlineRandom_LDFLAGS = $(LIAOUTLINER_LIBS)

outlineSelection_SOURCES = outlineSelection.cc
outlineSelection_LDFLAGS = $(LIAOUTLINER_LIBS)


# Import a list of C++ test codes expected to pass.
include $(TEST_DIR)/Makefile-pass.inc
if ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
else # !ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)
endif # ROSE_USE_LONG_MAKE_CHECK_RULE

# Outliner utility and flags to run with the tests.
OUTLINER_BIN = ./outlineSelection$(EXEEXT)
TEST_CXXFLAGS = $(TEST_INCLUDES) -rose:verbose 2 -rose:outline:seq 7


EXTRA_DIST = build_tests-1pass.sh RandomStmtSelector.hh 
CLEANFILES = Makefile-1pass LIST.dat

check-local:
	@echo "Test for ROSE outliner."
	$(MAKE) one-pass
	test -f Makefile-1pass
	$(MAKE) -f Makefile-1pass \
	    TEST_DIR="$(TEST_DIR)" \
	    OUTLINER_BIN="$(OUTLINER_BIN)" \
	    TEST_CXXFLAGS="$(TEST_CXXFLAGS)"
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astOutliningTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C *.C.pdf *.c.pdf

one-pass: Makefile-1pass $(OUTLINER_BIN)

LIST.dat: $(TEST_DIR)/Makefile-pass.inc
	echo $(TESTCODES_REQUIRED_TO_PASS) > LIST.dat

Makefile-1pass: ./build_tests-1pass.sh LIST.dat
	$(srcdir)/build_tests-1pass.sh $(top_srcdir) LIST.dat

# eof
