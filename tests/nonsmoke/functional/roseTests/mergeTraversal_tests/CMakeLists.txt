
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(traversalArray_SRCS traversalArray.C)

kde4_add_executable(traversalArray ${traversalArray_SRCS})

target_link_libraries(traversalArray ${KDE4_KDECORE_LIBS})

install(TARGETS traversalArray DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## To keep compile commands tidy, please ONLY put required include flags here.
#INCLUDES = $(ROSE_INCLUDES)
#
## "if" for DQ_DEVELOPER_TESTS (AutoMake conditional block)
#if DQ_DEVELOPER_TESTS
#
#bin_PROGRAMS = traversalArray
#
## This appears to be a generic translator
#traversalArray_SOURCES = traversalArray.C
#
## DQ (8/14/2004) Developer's Note:
## This permits the use of the pair of libraries -lrose -ledg OR the longer 
## list of libraries (the later option is much faster for development work).
## LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
#
## Turn on GNU Coverage Tool (gcov)
## CXX += -fprofile-arcs -ftest-coverage
#
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
## VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
## This was a bug specific to the normalization of source file names to absolute paths
#testTraversalArray: traversalArray
#	./traversalArray -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/input_traversalArray.C
#
#testRoseTraversalArray: traversalArray
#	./traversalArray -rose:verbose 2 -I$(top_builddir) $(INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
#
#check-local:
#	@echo "Dan Quinlan's development tests."
#	@echo "******************************************************************************************************************"
#	@echo "****** ROSE/developersScratchSpace/Dan/mergeTraversal_tests: make check rule complete (terminated normally) ******"
#	@echo "******************************************************************************************************************"
#else
#check-local:
#	@echo "\nDan Quinlan's Merged Traversal development tests NOT run.  To run these reconfigure with:"
#	@echo "*******************************************************************************************"
#	@echo "************* make check not run here: configure --enable-dq-developer-tests **************"
#	@echo "*******************************************************************************************"
#endif
#
#distclean-local:
#	rm -rf Makefile
#
#clean-local:
#	rm -rf *.o *.ps *.dot rose_*.C rose_*.c core *.int.c a.out 
#
#cleandata:
#	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o
#
#
