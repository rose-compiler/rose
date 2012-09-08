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

../src/vectorization:
	$(MAKE) -C $(top_builddir)/projects/vectorization/src

.PHONY: $(TESTCODES)
$(TESTCODES): ../src/vectorization
	../src/vectorization -msse3 -lsimd $(srcdir)/$@

$(COMPARE_CODES): $(TESTCODES)
	@if $(DIFF) $(top_srcdir)/projects/vectorization/tests/$@ $@; then \
		echo "vectorization: diff translated ouput: " $@;   \
	else    \
		echo "Files differ; test falied"; \
		exit 1; \
	fi

check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(TESTCODES)
	@$(MAKE) $(COMPARE_CODES)
else
	@echo "Skipping tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/vectorization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out 
EXTRA_DIST = $(TESTCODES) $(COMPARE_CODES)
