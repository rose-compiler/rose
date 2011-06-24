ALL_PYTHON_TESTCODES = $(wildcard $(srcdir)/*.py)

TEST_Objects = $(notdir ${ALL_PYTHON_TESTCODES:.py=.py-o})

$(TEST_Objects): ../../testTranslator
	../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.py-o=.py) -rose:output $@

../../testTranslator:
	$(MAKE) -C ../.. testTranslator

check-local:
	@echo "Testing Python."
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "***** ROSE/tests/CompileTests/Python_tests: make check rule complete (terminated normally) ****"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *-o

EXTRA_DIST = $(wildcard $(srcdir)/test*.py)
