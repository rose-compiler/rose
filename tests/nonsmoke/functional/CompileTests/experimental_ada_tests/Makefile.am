include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
	dot_asis_tests \
  compile_tests \
  sageInterfaceAda_tests \
  literalParser_tests \
  preprocessor_tests \
  configurationPragma_tests \
  ROSEGprIntegration_tests

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)
ROSE_FLAGS ?= -rose:skipfinalCompileStep -rose:ada:fail_hard_adb 

# ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler
ROSE_COMPILER ?= $(abs_builddir)/../../testTranslator

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

include $(srcdir)/passing-tests-list.mk
#~ include $(srcdir)/passing-incomplete-tests-list.mk
#~ include $(srcdir)/rose-backend-todo-list.mk
#~ include $(srcdir)/rose-frontend-todo-list.mk
#~ include $(srcdir)/asis-todo-list.mk
include $(srcdir)/AdaTest.mk

#NOT_SEPARATELY_CHECKED_DEPENDENCIES := \
#  comm.ads                             \
#  mpi.ads                              \
#  EndOfList

#
# define test suite
ROSE_OUTPUT_DIR := rose-ada-output
# ROSE_OUTPUT_DIR := .

TEST_TARGETS := $(addprefix $(ROSE_OUTPUT_DIR)/,$(ROSE_PASSING_ADA_TESTS))


TESTCODES_REQUIRED_TO_PASS := $(TEST_TARGETS)

#
# targets
.PHONY: all
all: 

#~passing

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: $(ROSE_OUTPUT_DIR)/EndOfList
$(ROSE_OUTPUT_DIR)/EndOfList:

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

#compile_tests/$(ROSE_OUTPUT_DIR)/default.gpr: $(srcdir)/tests/default.gpr
#	cp $< $@

$(abs_top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler

# testTranslator runs the AST consistency checks
$(abs_builddir)/../../testTranslator:
	$(MAKE) -C../.. testTranslator


compile_tests/$(ROSE_OUTPUT_DIR)/%:
	make -C compile_tests $(ROSE_OUTPUT_DIR)/%

$(ROSE_OUTPUT_DIR)/%.ads: compile_tests/$(ROSE_OUTPUT_DIR)/%.ads $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

$(ROSE_OUTPUT_DIR)/%.adb: compile_tests/$(ROSE_OUTPUT_DIR)/%.adb $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

check-test-number:
	@test "$(words $(ADA_TESTS))" = "$(shell ls $(srcdir)/tests/*.ad[b,s] | wc -l)" \
         || { echo $(shell ls $(srcdir)/tests/*.ad[b,s] | wc -l) tests available but $(words $(ADA_TESTS)) used; exit 2; } \
         && { echo Total $(words $(ADA_TESTS)) test files used for Ada testing; }

.PHONY: clean-local
clean-local:
	rm -rf ./rose-ada-output ./obj
	rm -f *.dot

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
	@$(MAKE) check-test-number
	@$(MAKE) $(PASSING_TEST_Objects_ads)
	@$(MAKE) $(PASSING_TEST_Objects_adb)
#	TOTALFILE=$(words $(ADA_TESTS))
#	TOTALFILE=$(shell  ls $(srcdir)/tests/*.ad[b,s] | wc -l )
#	TOTALADSFILE=$(words $(filter %.ads,$(ADA_TESTS)))
#	TOTALADBFILE=$(words $(filter %.adb,$(ADA_TESTS)))
#	TOTALROSEPASSFILE=$(words $(filter %.adb %.ads,$(ROSE_PASSING_ADA_TESTS)))
#	TOTALASISPASSFILE=$(words $(filter %.adb %.ads,$(ASIS_PASSING_TESTS)))
#	FEDOTFILE=$(shell  ls $(builddir)/dot_asis_tests/*.ad[b,s].dot | wc -l )
#	BEOUTFILE=$(shell  ls $(builddir)/compile_tests/rose-ada-output/*.ad[b,s] | wc -l )
	@echo "$(words $(ADA_TESTS)) total Ada tests"
	@echo "$(words $(ASIS_PASSING_TESTS)) passing ASIS tests"
	@echo "$(words $(ROSE_PASSING_ADA_TESTS)) passing ROSE AST tests"
	@echo "$(words $(ADA_TRANSLATOR_PASSING_TESTS)) passing Ada-to-C++ tests"

	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

