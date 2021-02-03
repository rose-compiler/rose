include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS    ?= -rose:skipfinalCompileStep --asis:process_predefined_units --asis:process_implementation_units


# ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler
ROSE_COMPILER ?= $(abs_builddir)/../../../testTranslator

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
# rose-backend-todo-list, rose-frontend-todo-list.mk, asis-todo-list.mk
# are not included, b/c they are currently not tested.

include $(srcdir)/../AdaTest.mk
include $(srcdir)/../passing-tests-list.mk
include $(srcdir)/../passing-incomplete-tests-list.mk
#~ include $(srcdir)/../rose-backend-todo-list.mk
#~ include $(srcdir)/../rose-frontend-todo-list.mk
#~ include $(srcdir)/../asis-todo-list.mk

NOT_SEPARATELY_CHECKED_DEPENDENCIES := \
  parent.ads                           \
  comm.ads                             \
  mpi.ads                              \
  EndOfList

#
# define test suite
ROSE_OUTPUT_DIR := rose-ada-output
# ROSE_OUTPUT_DIR := .

TEST_TARGETS := $(ROSE_OUTPUT_DIR)/default.gpr
TEST_TARGETS += $(addprefix $(ROSE_OUTPUT_DIR)/,$(ROSE_PASSING_ADA_TESTS))
TEST_TARGETS += $(addprefix $(ROSE_OUTPUT_DIR)/,$(ROSE_PASSING_IMCOMPLETE_ADA_TESTS))

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

# copy default.gpr needed by final full-circle test
$(ROSE_OUTPUT_DIR)/default.gpr: $(srcdir)/../tests/default.gpr
	mkdir -p $(ROSE_OUTPUT_DIR)
	cp $< $@

# rose-compiler does not run AST tests
$(abs_top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler

# testTranslator runs the AST consistency checks
$(abs_builddir)/../../../testTranslator:
	cd ../../..; $(MAKE) testTranslator

$(ROSE_OUTPUT_DIR)/%.ads:$(abs_srcdir)/../tests/%.ads $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

$(ROSE_OUTPUT_DIR)/%.adb:$(abs_srcdir)/../tests/%.adb $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

print-file-summary:
	@echo "Output Makefile variables:"
	@echo "ROSE_PASSING_ADA_TESTS = $(ROSE_PASSING_ADA_TESTS)"
	@echo "ROSE_PASSING_INCOMPLETE_ADA_TESTS = $(ROSE_PASSING_INCOMPLETE_ADA_TESTS)"
	@echo "ROSE_FAILING_ADA_TESTS = $(ROSE_FAILING_ADA_TESTS)"
	@echo "PASSING_TESTS = $(PASSING_TESTS)"
	@echo "PASSING_INCOMPLETE_TESTS = $(PASSING_INCOMPLETE_TESTS)"

.PHONY: clean-local
clean-local:
	rm -rf ./rose-ada-output ./obj
	rm -f *.dot
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

