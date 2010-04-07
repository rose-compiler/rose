include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# INCLUDES = $(ROSE_INCLUDES) -I$(srcdir) 

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = testSpuriousOutput_1.C testSpuriousOutput_2.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "Test compilation for any output (should be zero!)"
	@echo "#################################################"
	@$(MAKE) $(TEST_Objects)
	@echo "******************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testForSpuriousOutput: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************************"

# Test the -E option with all of these example files
$(TEST_Objects): $(TESTCODES)
	@rm -f outputFile; \
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.C) >& outputFile; \
	outputLines=`wc -l < outputFile`; \
	echo "Spurious lines of output = $$outputLines"; \
	if test $$outputLines -eq 0; then \
      echo "#############################################################################"; \
      echo "PASSED: Output from translation is quiet (no output detected from translator)"; \
      echo "#############################################################################"; \
   else \
      echo "####################################################################################################"; \
      echo "FAILED: Output from compilation detected (clean up translator to eliminate output when verbose == 0)"; \
      echo "####################################################################################################"; \
      exit 1; \
   fi;

EXTRA_DIST = \
   $(TESTCODES)

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






