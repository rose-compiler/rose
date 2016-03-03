include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR=$(top_builddir)/tests/testTranslator

$(TEST_TRANSLATOR):
	$(MAKE) -C $(top_builddir)/tests testTranslator

if ROSE_USING_GCC_VERSION_LATER_4_8
check-local: $(TEST_TRANSLATOR)
	TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX)  $(top_srcdir)/tests/CompileTests/STL_tests/stl-eval.sh
else
check-local:
        @echo "ROSE/tests/CompileTests/STL_tests: make check rule skipped (not using g++ 4.8+ compiler)."
endif
