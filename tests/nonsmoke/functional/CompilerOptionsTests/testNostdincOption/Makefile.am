include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES = test.C 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

TEST_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator $(ROSE_FLAGS)

$(TEST_TRANSLATOR):
	$(MAKE) testTranslator -C $(top_builddir)/tests/nonsmoke/functional

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "Test use of nostdinc  option ..."
	@echo "#################################################"
	@$(MAKE) testUseOfNostdincOption
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testNostdincOption: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************"

testUseOfNostdincOption: test.C $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) -nostdinc $(srcdir)/test.C 

EXTRA_DIST = $(TESTCODES)

clean-local:
	rm -f *.o rose_*.C a.out






