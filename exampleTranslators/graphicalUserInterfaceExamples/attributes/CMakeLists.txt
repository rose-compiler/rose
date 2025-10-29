
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############

set(attributes_SRCS attrib.cpp)

kde4_add_executable(attributes ${attributes_SRCS})

target_link_libraries(attributes ${KDE4_KDECORE_LIBS})

install(TARGETS attributes DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_QROSE
#bin_PROGRAMS = attributes
#
#attributes_SOURCES = attrib.cpp
#
## Use the names of the libraries as the list of dependences
#attributes_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#
## Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
#INCLUDES = $(ROSE_INCLUDES)
#
#
#EXTRA_DIST = $(TESTCODES)
#
#CLEANFILES = 
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files rose_*.C
#
#check-local:
#	rm -f test_01.o
#	$(MAKE) $(TEST_Objects)
#
#TESTCODES = 
##     test_01.cpp
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
#$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN attributes
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
##	DQ (8/13/2005): I have commented out these test since they don't terminate 
##	automatically and as a result are difficult to handle with the "make check" rule.
#	echo "testing of attributes GUI commented out since it won't terminate automatically"
##	./attributes -I$(srcdir) $(ROSE_FLAGS) -c $(srcdir)/../$(@:.o=.C)
#	: remove this so that we can force the examples to be run
#	rm -f $(TEST_Objects)
#
#test: attributes
#	./attributes -I$(srcdir) $(ROSE_FLAGS) -c $(srcdir)/../test_01.cpp
#endif
#
