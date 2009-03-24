include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "#################################################"
	@echo "    Test ROSEs Deleting of nodes                 "
	@echo "#################################################"

	@echo "*****************************************************************************************************************"
	@echo "*  ROSE/tests/RunTests/AstDeleteTests: make check rule complete (terminated normally)                           *"
	@echo "*****************************************************************************************************************"


EXTRA_DIST = 

clean-local:
	rm -f *.o test1 *.s rm *.out rose_*.C *.C.pdf *.dot core local_test-isystemOption.C *.dump *.new

