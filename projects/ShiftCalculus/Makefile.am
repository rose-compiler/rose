include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/23/2013): Disable so that we can try an approach modifiying the original ATerm library to be ROSE specific.
# DQ (10/20/2013): Allow alternative support using ROSE ATerm library API.
# SUBDIRS = tests

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = shiftCalculusCompiler 

shiftCalculusCompiler_SOURCES = shiftCalculusCompiler.C stencilAndStencilOperatorDetection.C stencilEvaluation.C stencilFiniteStateMachine.C dslSupport.C dslCodeGeneration.C lowLevelCodeGeneration.C

include_HEADERS = shiftCalculusCompiler.h stencilAndStencilOperatorDetection.h stencilEvaluation.h stencilFiniteStateMachine.h dslSupport.h dslCodeGeneration.h lowLevelCodeGeneration.h

ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = test2014_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

PASSING_REGRESSION_TEST_Output = ${TESTCODES_REQUIRED_TO_PASS:.C=.C.passing}


$(PASSING_REGRESSION_TEST_Output): ./shiftCalculusCompiler
	./shiftCalculusCompiler $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passing=.C)



test1: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/identity.cpp

test2: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/laplacian.cpp


test3: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/identity_lite.cpp

test4: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/laplacian_lite.cpp

test5: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_02.C

test6: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_03.C

test7: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_04.C

test8: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_05.C

# Note that test1 and test3 are zero dimensional stencils that are not supported for code generation yet.
# test2 is including STL and other library code
# test4 is a version of test2 which excludes STL and library code (and it thus smaller to work with but the same API).
# test5 - test8 test subsets of the stencil specification.
localTests: test4 test2 test5 test6 test7 test8


check-local:
	@echo "Tests for Shift Calculus examples."
#	@$(MAKE) $(PASSING_TEST_Output)
	@$(MAKE) $(PASSING_REGRESSION_TEST_Output)
	@$(MAKE) localTests
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: make check rule complete (terminated normally) ********"
	@echo "********************************************************************************************"


EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf rose_*.C *.pdf
	rm -f a.out *.dot 

