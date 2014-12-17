include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#DIFF=diff

TESTCODES_REQUIRED_TO_PASS = \
	jacobi.c \
	stencil-CNS.c \
	stream.c\
	inputloopTiling.c


TEST_OUTPUTS = $(TESTCODES_REQUIRED_TO_PASS:.c=)
COMPARE_OUTPUTS = $(COMPARE_CODES:.c=)

../src/mlm:
	$(MAKE) -C $(top_builddir)/projects/mlm/src

.PHONY: $(TEST_OUTPUTS)
$(TEST_OUTPUTS): ../src/mlm
	../src/mlm -rose:skipfinalCompileStep $(srcdir)/$(@:=.c)

#$(COMPARE_OUTPUTS): 
#	if $(DIFF) $(top_srcdir)/projects/mlm/tests/$(@:=.c) $(@:=.c); then \
#		echo "mlm: diff translated ouput: " $@;   \
#	else    \
#		echo "Files differ; test falied"; \
#		exit 1; \
#	fi

check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(TEST_OUTPUTS)
#	@$(MAKE) $(COMPARE_OUTPUTS)
else
	@echo "Skipping tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/mlm/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out 
EXTRA_DIST = $(TESTCODES_REQUIRED_TO_PASS) $(COMPARE_CODES)
