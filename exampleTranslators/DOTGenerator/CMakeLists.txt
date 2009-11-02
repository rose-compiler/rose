
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(dotGenerator_SRCS dotGenerator.C)

kde4_add_executable(dotGenerator ${dotGenerator_SRCS})

target_link_libraries(dotGenerator ${KDE4_KDECORE_LIBS})

install(TARGETS dotGenerator ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(dotGeneratorWholeASTGraph_SRCS dotGeneratorWholeASTGraph.C)

kde4_add_executable(dotGeneratorWholeASTGraph ${dotGeneratorWholeASTGraph_SRCS})

target_link_libraries(dotGeneratorWholeASTGraph ${KDE4_KDECORE_LIBS})

install(TARGETS dotGeneratorWholeASTGraph ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#bin_PROGRAMS = dotGenerator dotGeneratorWholeASTGraph
#
#dotGenerator_SOURCES = dotGenerator.C
#
#dotGeneratorWholeASTGraph_SOURCES = dotGeneratorWholeASTGraph.C
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
## changes to the templates in ../src/midend/astRewriteMechanism/rewriteTreeTraversal.C
## should force several files using those templates to be recompiled.
## Express this explicitly:
## dotGenerator.o: $(top_builddir)/src/midend/astRewriteMechanism/rewriteTreeTraversal.o
#
#EXTRA_DIST = $(TESTCODES)
#
#CLEANFILES = testdot_01.C.Preorder.dot testdot_01.C.Postorder.dot testdot_01.C.TopDown.dot testdot_01.C.BottomUp.dot testdot_01.C.TopDownBottomUp.dot
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf
#
#check-local:
#	rm -f testdot_01.o
#	$(MAKE) $(TEST_Objects)
#
#TESTCODES = \
#     testdot_01.C
#
## Build the list of object files
#TEST_Objects = ${TESTCODES:.C=.o}
#
#ROSE_FLAGS =
#
## This rule generates a dependency that forces the "make check" 
## test to always compile the test code
#FORCE_TEST_CODES_TO_RUN:
#
#$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN dotGenerator
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#	./dotGenerator -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#	./dotGeneratorWholeASTGraph -I$(srcdir) $(ROSE_FLAGS) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#
## DQ (1/4/2008): Required to cleanup for make distcheck
#distclean-local:
#	rm -rf *.dot
#
#
#
#
#
#
#
#
#
#
