include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )

add_executable (bug125 bug125.C)
add_executable (qualifiedName qualifiedName.C)

target_link_libraries(bug125 ROSE_DLL ${link_with_libraries} )
target_link_libraries(qualifiedName ROSE_DLL edg33 ${link_with_libraries} )

ENABLE_TESTING()
ADD_TEST(
  NAME testbug125
  COMMAND bug125 -c ${CMAKE_CURRENT_SOURCE_DIR}/inputbug125.C
)

ADD_TEST(
  NAME testqualifiedName
  COMMAND qualifiedName -c ${ROSE_TOP_SRC_DIR}/projects/autoTuning/tests/jacobi.c
)

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## To keep compile commands tidy, please ONLY put required include flags here.
#INCLUDES = $(ROSE_INCLUDES)
#
## The bug numbers come from ROSE's SciDAC bug trackers
#noinst_PROGRAMS = \
#       bug125 \
#       qualifiedName
#
#bug125_SOURCES = bug125.C
#qualifiedName_SOURCES = qualifiedName.C
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
#
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
#
#VALGRIND =
#
#testbug125:bug125 
#	./bug125 -c $(srcdir)/inputbug125.C
#
#testqualifiedName: qualifiedName
#	./qualifiedName -c $(top_srcdir)/projects/autoTuning/tests/jacobi.c
#
#EXTRA_DIST = CMakeLists.txt inputbug125.C
#
#check-local:
#	@$(MAKE) testbug125 testqualifiedName
#
#distclean-local:
#	rm -rf Makefile
#
#clean-local:
#	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C
#
#cleandata:
#	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o
#
