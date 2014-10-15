include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/23/2013): Disable so that we can try an approach modifiying the original ATerm library to be ROSE specific.
# DQ (10/20/2013): Allow alternative support using ROSE ATerm library API.
# SUBDIRS = tests

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = shiftCalculusCompiler 

shiftCalculusCompiler_SOURCES = shiftCalculusCompiler.C 

include_HEADERS = shiftCalculusCompiler.h

ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = test2013_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

PASSING_REGRESSION_TEST_Output = ${REGRESSION_TESTCODES:.C=.C.passing}


$(PASSING_REGRESSION_TEST_Output): ./shiftCalculusCompiler
	./shiftCalculusCompiler $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passing=.C)



test: shiftCalculusCompiler
	./shiftCalculusCompiler -c example1.C


check-local:
	@echo "Tests for Shift Calculus examples."
#	@$(MAKE) $(PASSING_TEST_Output)
	@$(MAKE) $(PASSING_REGRESSION_TEST_Output)
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: make check rule complete (terminated normally) ********"
	@echo "********************************************************************************************"


EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf rose_*.C *.pdf
	rm -f a.out *.dot 

