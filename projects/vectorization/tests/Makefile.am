include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

VECTORIZATION=../src/vectorization

DIFF=diff

TESTCODES = \
      simpleArithmetic.c \
      simpleIntegerArithmetic.c \
      multiDimensionArray.c \
	  FMA.c

COMPARE_CODES = \
      rose_simpleArithmetic.c \
      rose_simpleIntegerArithmetic.c \
      rose_multiDimensionArray.c \
	  rose_FMA.c

TEST_Results=$(TESTCODES:.c=.c)
TEST_COMPARE_FILES=$(COMPARE_CODES:.c=.c)

$(TEST_Results): $(VECTORIZATION) $(@:.c=.c)
	$(VECTORIZATION) -msse3 -lsimd $(srcdir)/$(@:.c=.c) -I$(exec_prefix)/include -L$(exec_prefix)/lib

$(TEST_COMPARE_FILES): $(TEST_Results) 
	@if $(DIFF)	$(srcdir)/$(@:.c=.c) $(@:.c=.c); then :; else echo "Files differ; test falied"; exit 1; fi

check-local:
	@$(MAKE) $(TEST_Results)
	@$(MAKE) $(TEST_COMPARE_FILES)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/vectorization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out 
EXTRA_DIST = $(TESTCODES)
