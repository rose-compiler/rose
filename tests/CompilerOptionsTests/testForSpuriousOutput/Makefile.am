include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# INCLUDES = $(ROSE_INCLUDES) -I$(srcdir) 

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
# TESTCODES = testSpuriousOutput_1.C testSpuriousOutput_2.C
CXX_TESTCODES = testSpuriousOutput_1.C testSpuriousOutput_2.C
F90_TESTCODES = testSpuriousOutput_3.f90 testSpuriousOutput_4.f90

# Build the list of object files
CXX_TEST_Objects = ${CXX_TESTCODES:.C=.o}
F90_TEST_Objects = ${F90_TESTCODES:.f90=.o}
TEST_Objects = ${C_TEST_Objects} ${F90_TEST_Objects}

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "Test compilation for any output (should be zero!)"
	@echo "#################################################"
	@$(MAKE) $(CXX_TEST_Objects)
	@$(MAKE) $(F90_TEST_Objects)
	@echo "******************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testForSpuriousOutput: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************************"

# Test the -E option with all of these example files
$(CXX_TEST_Objects): $(CXX_TESTCODES)
	@rm -f outputFile; \
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.C) >& outputFile; \
	outputLines=`wc -l < outputFile`; \
	echo "Spurious lines of output = $$outputLines"; \
	if test $$outputLines -eq 0; then \
      echo "#############################################################################"; \
      echo "PASSED: Output from C++ translation is quiet (no output detected from translator)"; \
      echo "#############################################################################"; \
   else \
      echo "####################################################################################################"; \
      echo "FAILED: Output from C++ compilation detected (clean up translator to eliminate output when verbose == 0)"; \
      echo "####################################################################################################"; \
      exit 1; \
   fi;

$(F90_TEST_Objects): $(F90_TESTCODES)
	@rm -f outputFile; \
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.f90) >& outputFile; \
	outputLines=`wc -l < outputFile`; \
	echo "Spurious lines of output = $$outputLines"; \
	if test $$outputLines -eq 0; then \
      echo "#############################################################################"; \
      echo "PASSED: Output from F90 translation is quiet (no output detected from translator)"; \
      echo "#############################################################################"; \
   else \
      echo "####################################################################################################"; \
      echo "FAILED: Output from F90 compilation detected (clean up translator to eliminate output when verbose == 0)"; \
      echo "####################################################################################################"; \
      exit 1; \
   fi;

EXTRA_DIST = $(CXX_TESTCODES) $(F90_TESTCODES)

clean-local:
	rm -f *.o rose_*.C rose_*.f rose_*.f?? *.sage.C test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






