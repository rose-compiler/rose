ALL_PYTHON_TESTCODES = \
    test2011_012.py \
    test2011_011.py \
    test2011_010.py \
    test2011_009.py \
    test2011_008.py \
    test2011_007.py \
    test2011_006.py \
    test2011_005.py \
    test2011_004.py \
    test2011_003.py \
    test2011_002.py \
    test2011_001.py \
    test2011_000.py

TEST_Objects = $(addsuffix .done, $(basename $(ALL_PYTHON_TESTCODES)))

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

EXTRA_DIST = $(ALL_PYTHON_TESTCODES)
