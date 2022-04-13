include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------

# DQ (9/19/2020): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

#------------------------------------------------------------------------------------------------------------------------
# testMerge executable
noinst_PROGRAMS += testDelete
testDelete_SOURCES = testDelete.C
testDelete_LDADD = $(ROSE_SEPARATE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# tests of the testMerge executable
testDelete_TESTS = testDelete_helloworld_cpp.passed
TEST_TARGETS += $(testDelete_TESTS)

.PHONY: check_testDelete
check_testDelete: $(testDelete_TESTS)

$(top_builddir)/src/hello-world.cpp:
	make -C $(top_builddir)/src check-hello-cxx

testDelete_helloworld_cpp.passed: testDelete $(top_builddir)/src/hello-world.cpp
	@$(RTH_RUN) \
		CMD="./testDelete $(top_builddir)/src/hello-world.cpp" \
		$(TEST_EXIT_STATUS) $@

testDelete_helloworld_cpp_ast.passed: testDelete $(top_builddir)/src/hello-world-cpp.ast
	@$(RTH_RUN) \
		CMD="./testDelete -rose:ast:read $(top_builddir)/src/hello-world-cpp.ast" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f mangleTest--mangleTwo.C.dot
