include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = dotGenerator dotGeneratorWholeASTGraph

dotGenerator_SOURCES = dotGenerator.C
dotGenerator_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

dotGeneratorWholeASTGraph_SOURCES = dotGeneratorWholeASTGraph.C
dotGeneratorWholeASTGraph_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# changes to the templates in ../src/midend/astRewriteMechanism/rewriteTreeTraversal.C
# should force several files using those templates to be recompiled.
# Express this explicitly:
dotGenerator.o: $(top_builddir)/src/midend/astRewriteMechanism/rewriteTreeTraversal.o

EXTRA_DIST = $(TESTCODES)

CLEANFILES = testdot_01.C.Preorder.dot testdot_01.C.Postorder.dot testdot_01.C.TopDown.dot testdot_01.C.BottomUp.dot testdot_01.C.TopDownBottomUp.dot

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf

check-local:
	rm -f testdot_01.o
	$(MAKE) $(TEST_Objects)

TESTCODES = \
     testdot_01.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS =

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN dotGenerator
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./dotGenerator -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./dotGeneratorWholeASTGraph -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

# DQ (1/4/2008): Required to cleanup for make distcheck
distclean-local:
	rm -rf *.dot










