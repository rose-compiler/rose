
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############

set(printVars_SRCS printVars.C MyTraversal.C)

kde4_add_executable(printVars ${printVars_SRCS})

target_link_libraries(printVars ${KDE4_KDECORE_LIBS})

install(TARGETS printVars DESTINATION bin)


########### install files ###############

install(FILES  MyTraversal.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#bin_PROGRAMS = printVars
#
#printVars_SOURCES = printVars.C MyTraversal.C
#printVars_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#
#
## testing (generate output, dot file, pdf file)
#
#TESTCODES = testInput1.C
#
#include_HEADERS = MyTraversal.h
#EXTRA_DIST = $(TESTCODES)
#
#CLEANFILES = testInput1.C.dot
#
#TEST_Objects = ${TESTCODES:.C=.o}
#
#check-local:
#	rm -f testInput1.o
#	$(MAKE) $(TEST_Objects)
#
## This rule generates a dependency that forces the "make check" 
## test to always compile the test code
#FORCE_TEST_CODES_TO_RUN:
#
#$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN printVars
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#	./printVars -I$(srcdir) $(ROSE_FLAGS) -DHAVE_CONFIG_H $(INCLUDES) -c $(srcdir)/$(@:.o=.C)
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
