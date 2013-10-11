include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = test_bool.C 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

$(TEST_TRANSLATOR):
	$(MAKE) testTranslator -C $(top_builddir)/tests

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "Test use of output file option ..."
	@echo "#################################################"
	@$(MAKE) testUseOfOutputFileOption
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testOutputFileOption: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************"

testUseOfOutputFileOption: test_bool.C $(TEST_TRANSLATOR)
	rm -f outputFile;
	$(TEST_TRANSLATOR) -c $(srcdir)/test_bool.C -o my_test_bool.o >& outputFile;

EXTRA_DIST = $(TESTCODES)

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






