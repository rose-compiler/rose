include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR=$(top_builddir)/tests/nonsmoke/functional/testTranslator

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

$(TEST_TRANSLATOR):
	$(MAKE) -C $(top_builddir)/tests/nonsmoke/functional testTranslator

if ROSE_USING_GCC_VERSION_LATER_4_8
# ASAN options are passed as arguments to allow to log them into a logfile
conditional-check-local: $(TEST_TRANSLATOR)
#	$(VALGRIND) ASAN_OPTIONS="$(VALGRIND)" TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX) $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh
if ROSE_USING_GCC_VERSION_LATER_4_9
if ROSE_USE_EDG_VERSION_4_9
# DQ (2/10/2017): Run tests using GNU 4.9 using only EDG 4.9 but disable C++11 specific testing.
	$(VALGRIND) ASAN_OPTIONS="$(VALGRIND)" TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX) CPP11_STL_TESTS=no $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh
else
# DQ (2/10/2017): Run tests using GNU 5.1 and later using only EDG 4.12
	$(VALGRIND) ASAN_OPTIONS="$(VALGRIND)" TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX) CPP11_STL_TESTS=yes $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh
endif
else
# DQ (2/10/2017): Run tests using GNU 4.8 and either EDG 4.9 or EDG 4.12
	$(VALGRIND) ASAN_OPTIONS="$(VALGRIND)" TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX) CPP11_STL_TESTS=yes $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh
endif
else
conditional-check-local:
        @echo "ROSE/tests/nonsmoke/functional/CompileTests/STL_tests: make check rule skipped (tests requires using g++ 4.8 compiler or later)."
endif

# DQ (1/19/2017): These are a problem for GNU 5.1.
# Markus and Dan discussed that these tests could not be enforce uniformally on newer compiler versions.
if !ROSE_USING_GCC_VERSION_LATER_5_1
check-local: conditional-check-local
else
check-local: 
endif
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/STL_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************"

check-no-cleanup: $(TEST_TRANSLATOR)
	$(VALGRIND) ASAN_OPTIONS="$(VALGRIND)" TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX)  $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh no-cleanup

clean-local:
	TOOL2=$(TEST_TRANSLATOR) TOOL1=$(CXX)  $(top_srcdir)/tests/nonsmoke/functional/CompileTests/STL_tests/stl-eval.sh only-cleanup
