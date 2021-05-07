include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

noinst_PROGRAMS := \
   testAdaLiteralParser

testAdaLiteralParser_SOURCES := testAdaLiteralParser.C

AM_CPPFLAGS := $(ROSE_INCLUDES) 
AM_LDFLAGS  := $(ROSE_RPATHS)
LDADD       := $(ROSE_LIBS)


# ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler
TEST_EXEC ?= $(abs_builddir)/testAdaLiteralParser

#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
#VALGRIND =


# Go back and build the translator we are using to test Fortran (in case make check
# is run directly in this directory before make has been run from the root of the
# ROSE directory structure).  This does not appear to work!


# $(TEST_CSHARP_Objects): ../../testTranslator
# if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
#	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:cs -c $(srcdir)/$(@:.o=.cs)
# endif

#
# include test lists
#
# passing-incomplete-tests-list, rose-backend-todo-list, rose-frontend-todo-list.mk,
# asis-todo-list.mk are not included, b/c they are currently not full-circle tested.


#
# define test suite

TESTCODES_REQUIRED_TO_PASS := literals.txt

#
# targets
.PHONY: 
all: testAdaLiteralParser

.PHONY: passing
passing: $(TEST_EXEC)
	$(TEST_EXEC) $(abs_srcdir)/$(TESTCODES_REQUIRED_TO_PASS)
 
.PHONY: print
print:
	@echo "Output Makefile variables: none"


.PHONY: clean-local
clean-local:

.PHONY: print-tests
print-tests:
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@echo "$(ADA_TESTS)"
else
	@echo "ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION is not set"
endif


.PHONY: check-local
check-local:
	@echo "Tests for experimental Ada frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@$(MAKE) passing
#	TOTALFILE=$(words $(ADA_TESTS))
#	TOTALFILE=$(shell  ls $(srcdir)/tests/*.ad[b,s] | wc -l )
#	TOTALADSFILE=$(words $(filter %.ads,$(ADA_TESTS)))
#	TOTALADBFILE=$(words $(filter %.adb,$(ADA_TESTS)))
#	TOTALROSEPASSFILE=$(words $(filter %.adb %.ads,$(ROSE_PASSING_ADA_TESTS)))
#	TOTALASISPASSFILE=$(words $(filter %.adb %.ads,$(ASIS_PASSING_TESTS)))
#	FEDOTFILE=$(shell  ls $(builddir)/dot_asis_tests/*.ad[b,s].dot | wc -l )
#	BEOUTFILE=$(shell  ls $(builddir)/compile_tests/rose-ada-output/*.ad[b,s] | wc -l )

	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/sageInterfaceAda_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

