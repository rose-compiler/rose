include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/23/2013): Disable so that we can try an approach modifiying the original ATerm library to be ROSE specific.
# DQ (10/20/2013): Allow alternative support using ROSE ATerm library API.
# SUBDIRS = tests

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = mapleDSLCompiler 

mapleDSLCompiler_SOURCES = mapleDSLCompiler.C operatorEvaluation.C operatorFiniteStateMachine.C dslSupport.C dslCodeGeneration.C callMaple.C mapleOperatorAPI.C lowLevelCodeGeneration.C stencilFiniteStateMachine.C

include_HEADERS = mapleDSLCompiler.h operatorEvaluation.h operatorFiniteStateMachine.h dslSupport.h dslCodeGeneration.h callMaple.h mapleOperatorAPI.h lowLevelCodeGeneration.h stencilFiniteStateMachine.h

MAPLE_LIB_DIR = $(MAPLE_INSTALL_PATH)/lib

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# DQ (10/27/2014): Added support for optional connection to Maple when ROSE is configured using --with-maple=/nfs/apps/maple/12
if ROSE_USE_MAPLE
   bin_PROGRAMS += omexample
   omexample_SOURCES = omexample.c
#  LDADD += -L$(MAPLE_LIBDIR) -lmaple -lmaplec
#  LDADD += -L$(MAPLE_LIBDIR) -lmaplec -lmaple
#  omexample_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -L$(MAPLE_LIBDIR) -lmaplec -lmaple
#  omexample_LDADD = -L$(MAPLE_LIBDIR) -lmaplec -lmaple -lmclient -lprocessor64
#  omexample_INCLUDES = $(MAPLE_INCLUDES)
   LDADD += -L$(MAPLE_LIBDIR) -lmaplec -lmaple -lmclient -lprocessor64
   AM_CPPFLAGS += $(MAPLE_INCLUDES)
endif

TESTCODES_REQUIRED_TO_PASS = test2014_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

PASSING_REGRESSION_TEST_Output = ${TESTCODES_REQUIRED_TO_PASS:.C=.C.passing}


$(PASSING_REGRESSION_TEST_Output): ./mapleDSLCompiler
	./mapleDSLCompiler $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passing=.C)

testMapleExample:

test1: mapleDSLCompiler
	./mapleDSLCompiler -c $(srcdir)/test2014_01.C

test2: mapleDSLCompiler
	./mapleDSLCompiler -c $(srcdir)/test2014_02.C

# Note test1 is a 2D Laplacian defined from first-order discretization operators.
# Note that test2 is a subset of test1 for testing.
localTests: test1


check-local:
	@echo "Tests for Maple DSL examples."
#	@$(MAKE) $(PASSING_TEST_Output)
	@$(MAKE) $(PASSING_REGRESSION_TEST_Output)
	@$(MAKE) localTests
	@echo "********************************************************************************************"
	@echo "********* ROSE/projects/MapleDLS: make check rule complete (terminated normally) ***********"
	@echo "********************************************************************************************"


EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf rose_*.C *.pdf
	rm -f a.out *.dot 

