include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = SizeofTest_GNU SizeofTest_Intel
SizeofTest_GNU_SOURCES = SizeofTest_GNU.C
SizeofTest_Intel_SOURCES = SizeofTest_Intel.C

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
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

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for sizeof operation."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/sizeofOperation_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"

clean-local:
	rm -f *.o test inputCode_SizeofTest
	rm -rf QMTest
