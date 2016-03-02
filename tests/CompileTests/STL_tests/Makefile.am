TEST_TRANSLATOR=$(top_builddir)/tests/testTranslator

check-local: $(TEST_TRANSLATOR)
	TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX)  $(top_srcdir)/tests/CompileTests/STL_tests/stl-eval.sh

$(TEST_TRANSLATOR):
	$(MAKE) -C $(top_builddir)/tests testTranslator

