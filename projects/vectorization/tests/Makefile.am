include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

DIFF=diff

TESTCODES_REQUIRED_TO_PASS = \
	simpleArithmetic.c \
	simpleIntegerArithmetic.c \
	multiDimensionArray.c \
	FMA.c

COMPARE_CODES = \
	rose_simpleArithmetic.c \
	rose_simpleIntegerArithmetic.c \
	rose_multiDimensionArray.c \
	rose_FMA.c

TEST_OUTPUTS = $(TESTCODES_REQUIRED_TO_PASS:.c=)
COMPARE_OUTPUTS = $(COMPARE_CODES:.c=)

../src/vectorization:
	$(MAKE) -C $(top_builddir)/projects/vectorization/src

.PHONY: $(TEST_OUTPUTS)
$(TEST_OUTPUTS): ../src/vectorization
	../src/vectorization $(srcdir)/$(@:=.c)

$(COMPARE_OUTPUTS): 
	@if $(DIFF) $(top_srcdir)/projects/vectorization/tests/$(@:=.c) $(@:=.c); then \
		echo "vectorization: diff translated ouput: " $@;   \
	else    \
		echo "Files differ; test falied"; \
		exit 1; \
	fi

check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(TEST_OUTPUTS)
#	@$(MAKE) $(COMPARE_OUTPUTS)
else
	@echo "Skipping tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/vectorization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out 
EXTRA_DIST = $(TESTCODES_REQUIRED_TO_PASS) $(COMPARE_CODES)
