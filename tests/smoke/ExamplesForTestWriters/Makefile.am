# This makefile serves as the canonical example for writing tests.
#
# Most of the documentation is in $ROSE/tests/nonsmoke/ExamplesForTestWriters/Makefile.am

########################################################################################################################
# Prologue

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/smoke/specimens/binary/Specimens.inc

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

########################################################################################################################
# A checker taking no specimens and which does not even depend on librose. It's only purpose is to see if it can be
# compiled and link; we never actually need to run it. (Don't remove this example -- it's a real test.)

noinst_PROGRAMS += basicLinkTest
basicLinkTest_SOURCES = basicLinkTest.C


########################################################################################################################
# A checker taking no specimens but which we want to run with a variety of arguments
# See $ROSE/tests/nonsmoke/specimens/binarySpecimens.inc

noinst_PROGRAMS += runAlgorithm
runAlgorithm_SOURCES = runAlgorithm.C

runAlgorithm_algorithms = a b c d

runAlgorithm_targets = $(addprefix runAlgorithm_, $(addsuffix .passed, $(runAlgorithm_algorithms)))
TEST_TARGETS += $(runAlgorithm_targets)

$(runAlgorithm_targets): runAlgorithm_%.passed: runAlgorithm
	@$(RTH_RUN)						\
		TITLE="algorithm $* [$@]"			\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/runAlgorithm --algorithm=$*"	\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-runAlgorithm
check-runAlgorithm: $(runAlgorithm_targets)

########################################################################################################################
# Boilerplate.

# check-smoke-local: $(TEST_TARGETS)
check-smoke-local:
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
	@echo "ExamplesForTestWriters tests using EDG 4.9 and GNU version 4.9 backend compiler are disabled."
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
#	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
	@$(MAKE) $(TEST_TARGETS)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
#	@echo "USING_CLANG_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9: Clang backend compiler are NOT allowed with the EDG 4.9 frontend."
else # NOT USING_CLANG_COMPILER
#	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
#	@echo "USING_INTEL_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
#	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
#	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ExamplesForTestWriters tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "ExamplesForTestWriters tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) $(TEST_TARGETS)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9: Clang backend compiler using EDG 4.12 frontend."
# DQ (2/28/2017): Skip these files with Clang for now and come back to them later (when we understand the limits of portability better).
	@$(MAKE) $(TEST_TARGETS)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) $(TEST_TARGETS)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "*****************************************************************************************************"
	@echo "****** ROSE/tests/smoke/ExamplesForTestWriters: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************"
