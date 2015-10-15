include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = printVars

printVars_SOURCES = printVars.C MyTraversal.C
printVars_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# testing (generate output, dot file, pdf file)

TESTCODES = testInput1.C

include_HEADERS = MyTraversal.h
EXTRA_DIST = CMakeLists.txt $(TESTCODES)

CLEANFILES = testInput1.C.dot

TEST_Objects = ${TESTCODES:.C=.o}

check-local:
	rm -f testInput1.o
	$(MAKE) $(TEST_Objects)

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN printVars
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./printVars -I$(srcdir) $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

clean-local:
	rm -rf Templates.DB ii_files ti_files
