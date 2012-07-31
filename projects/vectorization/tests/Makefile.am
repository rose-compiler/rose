include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

VECTORIZATION=../src/vectorization

TESTCODES = \
      simpleArithmetic.c \
      simpleIntegerArithmetic.c \
	  FMA.c

TEST_Results=\
	${TESTCODES:.c=.c}


$(TEST_Results): $(VECTORIZATION) $(@:.c=.c)
	$(VECTORIZATION) -msse3 -lsimd $(srcdir)/$(@:.c=.c) -I$(exec_prefix)/include -L$(exec_prefix)/lib
clean-local:
	rm -f *.o rose_*.[cC] 
EXTRA_DIST = $(TESTCODES)

check-local:
	@$(MAKE) $(TEST_Results)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/vectorization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

