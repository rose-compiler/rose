include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# traversalArray: this appears to be a generic translator
noinst_PROGRAMS += traversalArray
traversalArray_SOURCES = traversalArray.C

# This was a bug specific to the normalization of source file names to absolute paths
TEST_TARGETS += traversalArray1.passed
EXTRA_DIST += input_traversalArray.C
traversalArray1.passed: input_traversalArray.C traversalArray
	@$(RTH_RUN) \
		DISABLED="not tested in original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/traversalArray -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Pick a random large file...
TEST_TARGETS += traversalArray2.passed
traversalArray2.passed: $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C traversalArray
	@$(RTH_RUN) \
		DISABLED="not tested in original makefile" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/traversalArray -rose:verbose 0 -I$(top_builddir) $(AM_CPPFLAGS) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)

EXTRA_DIST += CMakeLists.txt
