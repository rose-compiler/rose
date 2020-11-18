include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
	dot_asis_tests \
  compile_tests

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS ?= -rose:skipfinalCompileStep --asis:process_predefined_units



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

NOT_SEPARATELY_CHECKED_DEPENDENCIES := \
  comm.ads                             \
  mpi.ads                              \
  EndOfList

#
# define test suite
ROSE_OUTPUT_DIR := rose-ada-output
# ROSE_OUTPUT_DIR := .

TEST_TARGETS := $(addprefix $(ROSE_OUTPUT_DIR)/,$(PASSING_TESTS))


TESTCODES_REQUIRED_TO_PASS := $(TEST_TARGETS)

#
# targets
.PHONY: all
all: passing

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

.PHONY: clean-local
clean-local:
	rm -rf ./rose-ada-output ./obj
	rm -f *.dot

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

