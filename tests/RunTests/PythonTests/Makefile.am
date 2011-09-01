ALL_PYTHON_TESTCODES = \
    test2011_037.py \
    test2011_036.py \
    test2011_035.py \
    test2011_034.py \
    test2011_033.py \
    test2011_032.py \
    test2011_031.py \
    test2011_030.py \
    test2011_029.py \
    test2011_028.py \
    test2011_027.py \
    test2011_026.py \
    test2011_025.py \
    test2011_024.py \
    test2011_023.py \
    test2011_022.py \
    test2011_021.py \
    test2011_020.py \
    test2011_019.py \
    test2011_018.py \
    test2011_017.py \
    test2011_016.py \
    test2011_015.py \
    test2011_014.py \
    test2011_013.py \
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

TEST_Objects = $(ALL_PYTHON_TESTCODES:.py=.test)

$(TEST_Objects): ../../testTranslator
    bname = $(basename $@)
	../../testTranslator $(srcdir)/$(bname).py > $(bname).py-ro
	@$(PYTHON) $(srcdir)/$(bname).py   > $(bname).py-o
	@diff $(bname).py-o $(bname).py-ro

../../testTranslator:
	$(MAKE) -C ../.. testTranslator

check-local:
	@echo "Testing Python."
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "******* ROSE/tests/RunTests/PythonTests: make check rule complete (terminated normally) *******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *-o rose_*.py *-ro

EXTRA_DIST = $(ALL_PYTHON_TESTCODES)
