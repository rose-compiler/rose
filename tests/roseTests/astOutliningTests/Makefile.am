# Import definitions common to Outliner submodules.
#include ../Make.inc
SUBDIRS = fortranTests

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# Default header file search path
INCLUDES = $(ROSE_INCLUDES)

# DQ (!/24/2008): We should not need this!
#        -I$(top_srcdir) -I$(top_builddir)
#       -I$(top_srcdir)/src/midend/astOutlining/src

# Default link flags for executables
OUTLINER_LIBS = $(ROSE_LIBS)

# Useful definitions for testing
#   TEST_DIR = Location of ROSE's basic C++ tests.
#   TEST_INCLUDES = Find header files upon which the test files depend.
TEST_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
TEST_INCLUDES = \
        -I$(TEST_DIR) \
        -I$(top_srcdir)/tests/CompileTests/A++Code
# end of ../Make.in----

# INCLUDES += -I.

noinst_PROGRAMS = \
	outline \
        injectOutlinePragmas \
        outlineRandom \
        outlineSelection

outline_SOURCES = outline.cc
outline_LDFLAGS = $(OUTLINER_LIBS)

injectOutlinePragmas_SOURCES = \
        RandomStmtSelector.cc \
        injectOutlinePragmas.cc
injectOutlinePragmas_LDFLAGS = $(OUTLINER_LIBS)

outlineRandom_SOURCES = \
        RandomStmtSelector.cc \
        outlineRandom.cc
outlineRandom_LDFLAGS = $(OUTLINER_LIBS)

outlineSelection_SOURCES = outlineSelection.cc
outlineSelection_LDFLAGS = $(OUTLINER_LIBS)


# Import a list of C++ test codes expected to pass.
include $(TEST_DIR)/Makefile-pass.inc
if ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
else # !ROSE_USE_LONG_MAKE_CHECK_RULE
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)
endif # ROSE_USE_LONG_MAKE_CHECK_RULE

# Outliner utility and flags to run with the tests.
OUTLINER_BIN = ./outlineSelection$(EXEEXT)
# this flag is used for the long list of CXX test files
TEST_CXXFLAGS = $(TEST_INCLUDES) -rose:verbose 0 -rose:outline:seq 7 -rose:outline:parameter_wrapper

# Additional tests using #pragma rose_outline, Liao, 12/17/2008
C_TESTCODES_REQUIRED_TO_PASS = \
	 moreTest1.c \
	 array1.c
CXX_TESTCODES_REQUIRED_TO_PASS = \
	array2.cpp \
	moreTest2.cpp

TESTCODE_CURRENTLY_FAILING =  	

ALL_EXTRA_TESTCODE = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# this flag is used for the files listed within this file: C_TESTCODES_REQUIRED_TO_PASS and CXX_TESTCODES_REQUIRED_TO_PASS
# TODO remove -rose:skipfinalCompileStep when the command line issue is solved
OUTLINE_FLAG= --edg:no_warnings -rose:verbose 0 -rose:outline:parameter_wrapper -rose:outline:new_file -rose:skipfinalCompileStep
 

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)
PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c outline 
	./outline$(EXEEXT) $(OUTLINE_FLAG) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp outline
	./outline$(EXEEXT) ${OUTLINE_FLAG} -c $<


EXTRA_DIST = build_tests-1pass.sh RandomStmtSelector.hh $(ALL_EXTRA_TESTCODE)

CLEANFILES = Makefile-1pass LIST.dat

# DQ (8/29/2008): Added dependences
check-local: outline$(EXEEXT) injectOutlinePragmas outlineRandom outlineSelection
	@echo "Test for ROSE outliner."
	$(MAKE) $(PASSING_C_TEST_Objects)
	$(MAKE) $(PASSING_CXX_TEST_Objects)
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
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.C.pdf *.c.pdf OUT_*

one-pass: Makefile-1pass $(OUTLINER_BIN)

LIST.dat: $(TEST_DIR)/Makefile-pass.inc
	echo $(TESTCODES_REQUIRED_TO_PASS) > LIST.dat

Makefile-1pass: ./build_tests-1pass.sh LIST.dat
	$(srcdir)/build_tests-1pass.sh $(top_srcdir) LIST.dat

# eof
