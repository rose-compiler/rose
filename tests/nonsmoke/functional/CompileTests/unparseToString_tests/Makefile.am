include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

########################################################################################################################
# Executables.
########################################################################################################################
noinst_PROGRAMS =

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += unparseToString
unparseToString_SOURCES = unparseToString.C
unparseToString_CPPFLAGS = $(ROSE_INCLUDES)
unparseToString_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

noinst_PROGRAMS += unparseProject
unparseProject_SOURCES = unparseProject.C
unparseProject_CPPFLAGS = $(ROSE_INCLUDES)
unparseProject_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

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
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
	@echo "unparseToString tests using GNU version 4.8 and earlier backend compiler."
	@$(MAKE) $(TEST_TARGETS)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
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
endif # ROSE_USE_EDG_VERSION_4_9
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
