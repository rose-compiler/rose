include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

TESTCODES_PASSING = \
   kripke-tpl-000.cxx \
   kripke-tpl-001.cxx \
   kripke-tpl-002.cxx \
   kripke-tpl-003.cxx \
   kripke-tpl-004.cxx \
   kripke-tpl-005.cxx \
   kripke-tpl-006.cxx

TESTCODES_FAILING = \
   kripke-tpl-007.cxx \
   doxygen-000.cxx \
   doxygen-001.cxx
   
TESTCODES = $(TESTCODES_PASSING) $(TESTCODES_FAILING)

PASSED_TESTCODES_PASSING = ${TESTCODES_PASSING:.cxx=.cxx.passed}
PASSED_TESTCODES = ${TESTCODES:.cxx=.cxx.passed}

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

TEST_TRANSLATOR=../../testTranslator

ROSE_FLAGS = -std=c++ -rose:unparse_template_ast -DSKIP_ROSE_BUILTIN_DECLARATIONS

$(PASSED_TESTCODES): %.passed: $(srcdir)/% $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) \
	   CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.cxx.passed=.cxx)" \
	   TITLE="TEST: $(TEST_TRANSLATOR) $(@:.cxx.passed=.cxx)" \
	   $(top_srcdir)/scripts/test_exit_status $@

check-local:
	@$(MAKE) $(PASSED_TESTCODES_PASSING)

status:
	@$(MAKE) $(PASSED_TESTCODES)

clean-local:
	rm -f *.o rose_*.cxx
	rm -f *.passed
	rm -f *.failed

EXTRA_DIST = $(TESTCODES)

