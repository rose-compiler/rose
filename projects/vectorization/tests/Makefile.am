include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

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

../src/vectorization:
	$(MAKE) -C $(top_builddir)/projects/vectorization/src  

$(TEST_Results): ../src/vectorization $(@:.c=.c)
	../src/vectorization -msse3 -lsimd $(srcdir)/$(@:.c=.c) -I$(top_srcdir)/projects/vectorization/src -L$(top_builddir)/projects/vectorization/src

$(TEST_COMPARE_FILES): $(TEST_Results) 
	@if $(DIFF)	$(top_srcdir)/projects/vectorization/tests/$(@:.c=.c) $(@:.c=.c); then :; else echo "Files differ; test falied"; exit 1; fi

check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(TEST_Results)
	@$(MAKE) $(TEST_COMPARE_FILES)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/vectorization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
else
	@echo "Skipping tests"
endif

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out 
EXTRA_DIST = $(TESTCODES) $(COMPARE_CODES)
