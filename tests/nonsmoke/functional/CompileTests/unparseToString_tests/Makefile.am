include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

########################################################################################################################
# Executables.
########################################################################################################################
noinst_PROGRAMS =

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += unparseToString
unparseToString_SOURCES = unparseToString.C
unparseToString_CPPFLAGS = $(ROSE_INCLUDES)
unparseToString_LDFLAGS = $(ROSE_RPATHS)
unparseToString_LDADD = $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += unparseProject
unparseProject_SOURCES = unparseProject.C
unparseProject_CPPFLAGS = $(ROSE_INCLUDES)
unparseProject_LDFLAGS = $(ROSE_RPATHS)
unparseProject_LDADD = $(ROSE_SEPARATE_LIBS)

########################################################################################################################
# Tests.  We currently have two tests (which are both the same executable but invoked with different switches) that
# operate over a big list of specimens (*.C files all from a common directory).
########################################################################################################################

# The list of specimens (which must all be from one directory so as to not be ambiguous)
SPECIMEN_DIR = $(abs_top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests
##DOES_NOT_WORK## include $(SPECIMEN_DIR)/Makefile-pass.inc
include ../Cxx_tests/Makefile-pass.inc
SPECIMENS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
TEST_CONFIG = $(top_srcdir)/scripts/test_exit_status
TEST_INCLUDES = -I$(SPECIMEN_DIR) -I$(top_srcdir)/tests/nonsmoke/functional/CompileTests/A++Code

# The makefile targets for "make check"
unparseAll_TEST_TARGETS = $(addprefix ua_, $(addsuffix .passed, $(SPECIMENS)))
unparseType_TEST_TARGETS = $(addprefix ut_, $(addsuffix .passed, $(SPECIMENS)))
TEST_TARGETS = $(unparseAll_TEST_TARGETS) $(unparseType_TEST_TARGETS)

# A couple of phony targets for running only one test across all the specimens
.PHONY: check-unparseAll check-unparseType
check-unparseAll: $(unparseAll_TEST_TARGETS)
check-unparseType: $(unparseType_TEST_TARGETS)

# How to run each test across each specimen
$(unparseAll_TEST_TARGETS): ua_%.passed: $(SPECIMEN_DIR)/% $(TEST_CONFIG) unparseToString
	@$(RTH_RUN) CMD="./unparseToString --all --edg:no_warnings -w --edg:restrict $(TEST_INCLUDES) -c $<" $(TEST_CONFIG) $@
$(unparseType_TEST_TARGETS): ut_%.passed: $(SPECIMEN_DIR)/% $(TEST_CONFIG) unparseToString
	@$(RTH_RUN) CMD="./unparseToString       --edg:no_warnings -w --edg:restrict $(TEST_INCLUDES) -c $<" $(TEST_CONFIG) $@

test_unparseProject: unparseProject
	./unparseProject $(srcdir)/test2014_26.C

test_unparseProject_template: unparseProject
	./unparseProject $(srcdir)/test2017_01.C

########################################################################################################################
# Additional automake rules
########################################################################################################################

# check-local: $(TEST_TARGETS)
check-local:
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
	@echo "unparseToString tests using GNU version 4.8 backend compiler."
	@$(MAKE) $(TEST_TARGETS)
else  # NOT USING_GNU_COMPILER
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
# DQ (2/28/2017): Skipping these tests for now.
#	@$(MAKE) $(TESTCODES_Objects)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@$(MAKE) $(TEST_TARGETS)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/unparseToString_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"

# These "rm" commands are split up because the argument lists are too long for some shells.
clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(SPECIMENS:.C=.o)
	rm -f $(addprefix rose_, $(SPECIMENS))
