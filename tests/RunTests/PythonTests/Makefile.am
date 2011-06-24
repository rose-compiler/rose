ALL_PYTHON_TESTCODES = $(wildcard $(srcdir)/*.py)

TEST_Objects = $(addsuffix .done, $(basename $(notdir $(ALL_PYTHON_TESTCODES))))

$(TEST_Objects): ../../testTranslator
    bname = $(basename $@)
	@echo "  TEST $(bname).py"
	@../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(bname).py -rose:output $(bname).py-r
	@$(PYTHON) $(srcdir)/$(bname).py   > $(bname).py-o
	@$(PYTHON)           $(bname).py-r > $(bname).py-ro
	@diff $(bname).py-o $(bname).py-ro
	@touch $(bname).done

../../testTranslator:
	$(MAKE) -C ../.. testTranslator

check-local:
	@echo "Testing Python."
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "******* ROSE/tests/RunTests/PythonTests: make check rule complete (terminated normally) *******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *-o *-ro *-r *.done

EXTRA_DIST = $(wildcard $(srcdir)/test*.py)
