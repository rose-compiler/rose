include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

noinst_PROGRAMS := \
   testSageInterfaceAda

testSageInterfaceAda_SOURCES := testSageInterfaceAda.C

AM_CPPFLAGS := $(ROSE_INCLUDES)
AM_LDFLAGS  := $(ROSE_RPATHS)
LDADD       := $(ROSE_LIBS)



# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS ?= -rose:skipfinalCompileStep --asis:process_predefined_units --asis:process_implementation_units



# ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler
ROSE_COMPILER ?= $(abs_builddir)/testSageInterfaceAda

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

include $(srcdir)/../AdaTest.mk

#
# define test suite
ROSE_OUTPUT_DIR := rose-ada-output
# ROSE_OUTPUT_DIR := .

TEST_TARGETS0 := $(addprefix $(ROSE_OUTPUT_DIR)/,$(ROSE_PASSING_ADA_TESTS))
TEST_TARGETS1 := $(TEST_TARGETS0:.adb=.chk)
TEST_TARGETS  := $(TEST_TARGETS1:.ads=.chk)

TESTCODES_REQUIRED_TO_PASS := $(TEST_TARGETS)

#
# targets
.PHONY: all
all: passing

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

#compile_tests/$(ROSE_OUTPUT_DIR)/default.gpr: $(srcdir)/tests/default.gpr
#	cp $< $@

#~ compile_tests/$(ROSE_OUTPUT_DIR)/%:
#~ 	make -C compile_tests $(ROSE_OUTPUT_DIR)/%

$(ROSE_OUTPUT_DIR)/%.chk:$(abs_srcdir)/../tests/%.ads $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

$(ROSE_OUTPUT_DIR)/%.chk:$(abs_srcdir)/../tests/%.adb $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

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

	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/sageInterfaceAda_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

