include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = test-includeOption.C test-isystemOption.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "           Test use of GNU options ..."
	@echo "#################################################"
	@$(MAKE) testUseOfGnuOptions
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testGnuOptions: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************"

# TOO (2/18/11): Added to support CC/CXX environment variables when selecting compilers for ROSE.
# Otherwise, $(srcdir)/testOptions.h doesn't expand properly during make distcheck.
absolute_srcdir=`cd $(srcdir) && pwd`

testUseOfGnuOptions: $(srcdir)/test-includeOption.C $(srcdir)/test-isystemOption.C 
#	If -include is not handled properly by ROSE this will fail.
	$(TEST_TRANSLATOR) -rose:verbose 0 -include $(absolute_srcdir)/testOptions.h -c $(srcdir)/test-includeOption.C
#	Since the default is to search the source file directory, this is not a great test
	$(TEST_TRANSLATOR) -rose:verbose 0 -isystem $(srcdir) -c $(srcdir)/test-isystemOption.C
#	The default is to search the source file directory, so move the source file to 
#	be local and specify the path to the include file using -isystem. If -isystem 
#	is not handled properly by ROSE then this test will fail.
	cp $(srcdir)/test-isystemOption.C local_test-isystemOption.C
	$(TEST_TRANSLATOR) -rose:verbose 0 -isystem $(srcdir) -c local_test-isystemOption.C

test_skip_rose_option:
	$(TEST_TRANSLATOR) -rose:verbose 0 -isystem $(srcdir) -c $(srcdir)/test-isystemOption.C -rose:skip_rose
	$(TEST_TRANSLATOR) -rose:C --edg:no_warnings --edg:restrict -w  -c $(top_srcdir)/tests/CompileTests/C_tests/test2006_135.c -rose:skip_rose


EXTRA_DIST = $(TESTCODES) testOptions.h

clean-local:
	rm -f *.o rose_*.C *.C.pdf *.dot core local_test-isystemOption.C

