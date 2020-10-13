include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS    ?= -rose:skipfinalCompileStep

# testTranslator runs the AST consistency checks
../../testTranslator:
	cd ../..; $(MAKE) testTranslator


# ROSE_COMPILER ?= $(top_builddir)/src/rose-compiler
ROSE_COMPILER ?= ../../testTranslator

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

ADA_TEST_SRC_DIR := $(top_srcdir)/tests/nonsmoke/functioncal/CompileTests/experimental_ada_tests

include $(srcdir)/passing-tests-list.mk
include $(srcdir)/passing-incomplete-tests-list.mk
include $(srcdir)/rose-backend-todo-list.mk
include $(srcdir)/rose-frontend-todo-list.mk
include $(srcdir)/asis-todo-list.mk

NOT_SEPARATELY_CHECKED_DEPENDENCIES := \
  parent.ads                           \
  comm.ads                             \
  mpi.ads                              \
  EndOfList

#
# define test suite
TEST_TARGETS := $(PASSING_TESTS) $(PASSING_INCOMPLETE_TESTS)
ALL_TEST_TARGETS := $(PASSING_TESTS) $(PASSING_INCOMPLETE_TESTS) $(ROSE_BACKEND_TODO) $(ROSE_FRONTEND_TODO) $(ASIS_TODO)

TESTCODES_REQUIRED_TO_PASS := $(TEST_TARGETS)

#
# targets
.PHONY: all
all: passing

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: complete
complete: $(ALL_TEST_TARGETS)

.PHONY: passing-frontend
passing-frontend: $(PASSING_FRONTEND)

.PHONY: passing-asis
passing-asis: $(PASSING_ASIS)

.PHONY: EndOfList
EndOfList:

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

$(top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler

%.ads:$(abs_srcdir)/tests/%.ads $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

%.adb:$(abs_srcdir)/tests/%.adb $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

.PHONY: clean-local
clean-local:
	rm -f *.adb *.ads *.ali *.dot
	rm -f $(srcdir)/tests/obj/*

.PHONY: check-local
check-local:
	@echo "Tests for experimental Ada frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects_ads)
	@$(MAKE) $(PASSING_TEST_Objects_adb)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

