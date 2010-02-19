include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = CompileTests roseTests RunTests PerformanceTests CompilerOptionsTests translatorTests

# Three phases (levels) of test programs:
#  Phase 1) Analysis only, compiles the original code
#  Phase 2) Analysis and code generation, compiles the original code
#  Phase 3) analysis, code generation, and compiling the generated code
bin_PROGRAMS = testAnalysis testCodeGeneration testTranslator testAstFileIO

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

# This tests the ability to generate a compressed binary file representation of the AST.
# This is a special format of the ROSE AST build for performance File I/O (to address the
# requirements of large scale applications and their analysis).
testAstFileIO_SOURCES = testAstFileIO.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt inputFile.C tests.tex

# ********************************
# *******  Analysis Tests  *******
# ********************************
testObjectFileAnalysis: testAnalysis
	./testAnalysis $(INCLUDES) -c $(srcdir)/inputFile.C -o alt_Analysis_inputFile.o

testLinkFileAnalysis: testObjectFileAnalysis
#	./testAnalysis $(INCLUDES) alt_inputFile.o -o alt_inputFile
	./testAnalysis $(INCLUDES) alt_Analysis_inputFile.o -o testLinkFileAnalysisExecutable

testCppFileAnalysis: testAnalysis
	./testAnalysis -E $(INCLUDES) $(srcdir)/inputFile.C

testExecutableFileAnalysis: testAnalysis
	cp $(srcdir)/inputFile.C inputFileAnalysis.C
	./testAnalysis $(INCLUDES) inputFileAnalysis.C -o testExecutableFileAnalysisExecutable

test_testAnalysis: testObjectFileAnalysis testLinkFileAnalysis testCppFileAnalysis testExecutableFileAnalysis


# ****************************************
# *******  Code Generation Tests  ********
# ****************************************
testObjectFileCodeGeneration: testCodeGeneration
	./testCodeGeneration $(INCLUDES) -c $(srcdir)/inputFile.C -o alt_CodeGeneration_inputFile.o

testLinkFileCodeGeneration: testObjectFileCodeGeneration
	./testCodeGeneration $(INCLUDES) alt_CodeGeneration_inputFile.o -o testLinkFileCodeGenerationExecutable

testCppFileCodeGeneration: testCodeGeneration
	./testCodeGeneration -E $(INCLUDES) $(srcdir)/inputFile.C

testExecutableFileCodeGeneration: testCodeGeneration
	cp $(srcdir)/inputFile.C inputFileCodeGeneration.C
	./testCodeGeneration $(INCLUDES) inputFileCodeGeneration.C -o testExecutableFileCodeGenerationExecutable

test_testCodeGeneration: testObjectFileCodeGeneration testLinkFileCodeGeneration testCppFileCodeGeneration testExecutableFileCodeGeneration


# **********************************
# *******  Translator Tests  *******
# **********************************
testObjectFileTranslator: testTranslator
	./testTranslator $(INCLUDES) -c $(srcdir)/inputFile.C -o alt_Translator_inputFile.o

testLinkFileTranslator: testObjectFileTranslator
	./testTranslator $(INCLUDES) alt_Translator_inputFile.o -o testLinkFileTranslatorExecutable

testCppFileTranslator: testTranslator
	./testTranslator -E $(INCLUDES) $(srcdir)/inputFile.C

testExecutableFileTranslator: testTranslator
	cp $(srcdir)/inputFile.C inputFileTranslator.C
	./testTranslator $(INCLUDES) inputFileTranslator.C -o testExecutableFileTranslatorExecutable

test_testTranslator: testObjectFileTranslator testLinkFileTranslator testCppFileTranslator testExecutableFileTranslator


# ************************************
# *******  AST File I/O Tests  *******
# ************************************
testObjectFileFileAstFileIO: testAstFileIO
	cp $(srcdir)/inputFile.C alt_AstFileIO_inputFile.C
	./testAstFileIO $(INCLUDES) -c alt_AstFileIO_inputFile.C -o alt_AstFileIO_inputFile

test_testAstFileIO: testObjectFileFileAstFileIO

# Reported failure from Tristan
testSimpleLinkFileTranslator: testTranslator
	cp $(srcdir)/inputFile.C inputSimpleLinkFileTranslator.C
	./testTranslator inputSimpleLinkFileTranslator.C -lm



# Rule to run all the example translator tests
test: test_testAnalysis test_testCodeGeneration test_testTranslator test_testAstFileIO testSimpleLinkFileTranslator


# Make check rule
check-local: test
	@echo "************************************************************************"
	@echo "****** ROSE/tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************"


# Make clean rule
clean-local: myCleanRule

myCleanRule:
	rm -f testLinkFileAnalysisExecutable testLinkFileCodeGenerationExecutable testLinkFileTranslatorExecutable testExecutableFileAstFileIOExecutable
	rm -f testExecutableFileAnalysisExecutable testExecutableFileCodeGenerationExecutable testExecutableFileTranslatorExecutable
	rm -f rose_*.C inputFileTranslator.C inputFileCodeGeneration.C inputFileAnalysis.C inputSimpleLinkFileTranslator.C inputFileAstFileIO.C
	rm -f alt_AstFileIO_inputFile* a.out *.dot *.binary











