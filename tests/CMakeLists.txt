
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS} )

add_executable( testTranslator testTranslator.C )
target_link_libraries(testTranslator ROSE_DLL edg33 ${link_with_libraries} )

add_subdirectory (translatorTests)
enable_testing()

find_program(bash bash)
if(bash)
  add_test(
    NAME check_policies
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${bash} ${CMAKE_SOURCE_DIR}/scripts/policies-checker.sh)
endif()

# ${Boost_LIBRARIES}
#    libastMerge libsageInterface libastTokenStream 
#    libastHiddenTypeAndDeclarationLists libastVisualization 
#    roseDisassemblers
#    libsageInterface roseExecFormats midend unparser
#    libastHiddenTypeAndDeclarationLists roseAsmUnparser libastPostProcessing
#    libhpdf


#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## Temporarily remove the performance tests since they don't yet compile properly
## SUBDIRS = CompileTests RunTests PerformanceTests
## SUBDIRS = CompileTests RunTests @optional_PERFORMANCE_subdirs@
## if ROSE_PERFORMANCE_TESTS
##      SUBDIRS = CompileTests RunTests PerformanceTests
## else
##      SUBDIRS = CompileTests RunTests
## endif
#
#SUBDIRS = CompileTests roseTests RunTests PerformanceTests CompilerOptionsTests \
#           translatorTests
#
## CompileTests
#
## This allows us to make distributions that could exclude the PerformanceTests directory
## I don't know if this is a good idea but it also allows us to test ROSE without the
## PerformanceTests directory as well.
## DIST_SUBDIRS = $(SUBDIRS)
#
## DQ (1/7/2004): Relocation of ROSE/src/rose.C to ROSE/tests/testTranslator.C
## Build rose.C test translator in this directory since it is only used for tests
## of the included test codes.  This code has been moved from the ROSE/src/rose.C
## to ROSE/tests/testTranslator.C as part of the directory reorganization.
## The change was required because of the dependences of ROSE/src/rose.C upon
## libraries that had not yet been built.  There was no order in which the
## parent directories could be visited to build all the libraries require before 
## the ROSE/src directory was processed.
#bin_PROGRAMS = testTranslator testAnalysis
#
## This tests the ability to read and regnerate the input application and compile it using the backend compiler.
#testTranslator_SOURCES = testTranslator.C
#
## This tests only the ability to read application, not regenerate the source code and compile it using the backend compiler.
## This test code is used for the testing of ROSE compiling ROSE to support analysis using Compass.
#testAnalysis_SOURCES = testAnalysis.C
#
## Use the names of the libraries as the list of dependences
##testTranslator_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
#
## Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
#INCLUDES = $(ROSE_INCLUDES)
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#EXTRA_DIST = tests.tex
#
#testObjectFile: testAnalysis
#	./testAnalysis $(INCLUDES) -c $(srcdir)/inputFile.C -o alt_inputFile.o
#
#testLinkFile: testObjectFile
##	./testAnalysis $(INCLUDES) alt_inputFile.o -o alt_inputFile
#	./testAnalysis $(INCLUDES) alt_inputFile.o
#
#testCppFile: testAnalysis
#	./testAnalysis -E $(INCLUDES) $(srcdir)/inputFile.C
#
#testExecutableFile: testAnalysis
#	./testAnalysis $(INCLUDES) $(srcdir)/inputFile.C
#
#check-local:
#	@echo "************************************************************************"
#	@echo "****** ROSE/tests: make check rule complete (terminated normally) ******"
#	@echo "************************************************************************"
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
#
#
#
#
#
