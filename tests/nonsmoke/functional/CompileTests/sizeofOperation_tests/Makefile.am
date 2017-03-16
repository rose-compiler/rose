include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = SizeofTest_GNU SizeofTest_Intel
SizeofTest_GNU_SOURCES = SizeofTest_GNU.C
SizeofTest_Intel_SOURCES = SizeofTest_Intel.C

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = inputCode_SizeofTest.C

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): SizeofTest_GNU $(srcdir)/$(@:.o=.C)
$(TEST_Objects): SizeofTest_GNU SizeofTest_Intel
	./SizeofTest_GNU -c $(srcdir)/$(@:.o=.C)
# DQ (10/21/2009): comment out for now while Yi fixes this.
	./SizeofTest_Intel -c $(srcdir)/$(@:.o=.C)

#QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): SizeofTest_GNU.C $(srcdir)/$(@:.qmt=.C)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/SizeofTest_GNU NULL -c $(srcdir)/$(@:.qmt=.C)
#$(QMTEST_Objects): SizeofTest_Intel.C $(srcdir)/$(@:.qmt=.C)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/SizeofTest_Intel NULL -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
#include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for sizeof operation."
# DQ (2/18/2017): More specific logic required for portability.
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_9
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
	@$(MAKE) $(PASSING_TEST_Objects)
endif # ROSE_USING_GCC_VERSION_LATER_4_9
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
	@$(MAKE) $(PASSING_TEST_Objects)
endif # ROSE_USE_EDG_VERSION_4_9
#	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/sizeofOperation_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"

clean-local:
	rm -f *.o test inputCode_SizeofTest
	rm -rf QMTest
