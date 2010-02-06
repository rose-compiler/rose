include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = CompileTests roseTests RunTests PerformanceTests CompilerOptionsTests translatorTests

# Three phases (levels) of test programs:
#  Phase 1) Analysis only, compiles the original code
#  Phase 2) Analysis and code generation, compiles the original code
#  Phase 3) analysis, code generation, and compiling the generated code
bin_PROGRAMS = testAnalysis testCodeGeneration testTranslator

# Phase 1 tests code:
# This is used in specialized tests outside of ROSE testing for analysis only of large projects.
# This tests only the ability to read the application, not regenerate the source code and compile it using the backend compiler.
# This test code is used for the testing of ROSE compiling ROSE to support analysis using Compass.
testAnalysis_SOURCES = testAnalysis.C

# Phase 2 tests code:
# This is used in specialized tests outside of ROSE testing (e.g. intermediate testing of ROSE compiling large projects).
# This test the ability to read the application and generate the source code, but uses the original source code 
# with the backend compiler to generate object files or executables.
testCodeGeneration_SOURCES = testCodeGeneration.C

# Phase 3 tests code (used in all internal tests of ROSE; regression tests):
# This tests the ability to read and regnerate the input application and compile it using the backend compiler.
testTranslator_SOURCES = testTranslator.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt tests.tex

testObjectFile: testAnalysis
	./testAnalysis $(INCLUDES) -c $(srcdir)/inputFile.C -o alt_inputFile.o

testLinkFile: testObjectFile
#	./testAnalysis $(INCLUDES) alt_inputFile.o -o alt_inputFile
	./testAnalysis $(INCLUDES) alt_inputFile.o

testCppFile: testAnalysis
	./testAnalysis -E $(INCLUDES) $(srcdir)/inputFile.C

testExecutableFile: testAnalysis
	./testAnalysis $(INCLUDES) $(srcdir)/inputFile.C

test: testObjectFile testLinkFile testCppFile testExecutableFile

check-local: testObjectFile testLinkFile testCppFile testExecutableFile
	@echo "************************************************************************"
	@echo "****** ROSE/tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************"















