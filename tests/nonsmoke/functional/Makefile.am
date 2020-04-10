include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

PHONY: test test_testTokenGeneration test_testAnalysis test_testCodeGeneration test_testTranslator test_testTemplates test_testGraphGeneration test_testPDFGeneration testSimpleLinkFileTranslator testTranslatorFoldedConstants testTranslatorUnfoldedConstants testKeepGoingTranslator test_exampleIdentityTranslator

include $(top_srcdir)/tests/nonsmoke/functional/input_codes/Makefile.for.input.codes

# DQ (4/17/2010): If we are not supporting C++ then we can't run most of our tests, but if 
# we are supporting only Fortran then we can at least run the Fortran tests (same with PHP).
# SUBDIRS = CompileTests roseTests RunTests PerformanceTests CompilerOptionsTests translatorTests
# SUBDIRS = CompileTests roseTests
# SUBDIRS += . UnitTests CompileTests

# DQ (10/21/2016): Change order of tests (don't test current directory first).
# SUBDIRS += . CompileTests UnitTests 
SUBDIRS += CompileTests UnitTests 

if !ROSE_USE_CLANG_FRONTEND

# JP (10/3/14): In order to be able to run the unit tests, we want to build the test support,
# this includes google test as well as our own header, providing some necessary helper functions
SUBDIRS += testSupport


#if !ROSE_USE_INSURE
# DQ (4/23/2011): Ignoring this whole directory might be over kill. But I have had problems, 
# only on Hudson, just compiling a number of these subdirectories.
   SUBDIRS += roseTests
#endif

SUBDIRS += RunTests
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += PerformanceTests CompilerOptionsTests translatorTests moveDeclarationTool
endif

endif

# DQ (10/21/2016): Change order of tests (Utility directory test should go after language tests).
########################################################################################################################
# Utility functions

SUBDIRS += Utility

########################################################################################################################
# Rose::BinaryAnalysis

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if !ROSE_USE_INSURE
# There are failing tests here (94 tests pass and 55 tests fail).
if !ROSE_USE_SANITIZER
SUBDIRS += BinaryAnalysis
endif
endif
endif

########################################################################################################################

# Three phases (levels) of test programs:
#  Phase 1) Analysis only, compiles the original code
#  Phase 2) Analysis and code generation, compiles the original code
#  Phase 3) analysis, code generation, and compiling the generated code

$(top_builddir)/exampleTranslators/defaultTranslator/KeepGoingTranslator: $(top_srcdir)/exampleTranslators/defaultTranslator/KeepGoingTranslator.cpp
	make -C $(top_builddir)/exampleTranslators/defaultTranslator

#  Non-installed programs
noinst_PROGRAMS = \
   testParser \
   testFrontEnd \
   testAnalysis \
   testCodeGeneration \
   testTranslator \
   testTranslatorUnfoldedConstants \
   testTranslatorFoldedConstants \
   testGraphGeneration \
   testTokenGeneration \
   testTemplates \
   testBackend \
   readFileTwice

if ROSE_WITH_LIBHARU
   noinst_PROGRAMS += testPDFGeneration
endif

if ROSE_WITH_ATERM
   noinst_PROGRAMS += testAtermGraphGeneration
endif

# Phase 0 tests code:
# This is a test of the EDG and OFP parser support. No significant part of ROSE is generated.
# A bit of the infrastructure of ROSE is tested that sets up work to build the ROSE AST, etc.
# This test only works with C/C++ and Fortran.  Not clear what it means for the binary, 
# which has its own options for skipping different phases.
testParser_SOURCES = testParser.C

# This test code just calls the EDG frontend and then compiles the 
# original source file using the backend specified to ROSE.  This is 
# useful in evaluating the newer EDG frontend against test codes.
testFrontEnd_SOURCES = testFrontEnd.C

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

# These tests an option to use frontend specific constant folding or the original expression trees (default is no constnt folding).
testTranslatorUnfoldedConstants_SOURCES = testTranslatorUnfoldedConstants.C
testTranslatorFoldedConstants_SOURCES   = testTranslatorFoldedConstants.C

# This tests the ability to generate dot file (AST and WholeGraph) for the input application.
testGraphGeneration_SOURCES = testGraphGeneration.C

if ROSE_WITH_LIBHARU
# This tests the ability to generate pdf file (AST) for the input application.
testPDFGeneration_SOURCES = testPDFGeneration.C
endif

# This tests the token generation capabilities which are an option in ROSE used to support the 
# generation of patches to define transformations of large scale applications.
testTokenGeneration_SOURCES = testTokenGeneration.C

if ROSE_WITH_ATERM
# DQ (9/28/2013): Added support for Aterm graph generation.
testAtermGraphGeneration_SOURCES = testAtermGraphGeneration.C
endif

# Ignores internal ROSE failures, see `rose --help` for info on `-rose:keep_going`
KeepGoingTranslator_SOURCES = KeepGoingTranslator.cpp

# DQ (7/2/2015): Added test that marks templates to be output with instantiations.
testTemplates_SOURCES = testTemplates.C

testBackend_SOURCES = testBackend.C

readFileTwice_SOURCES = readFileTwice.C

# DQ (7/3/2013): Added support to output some pre-defined macro values.
# Program to output predefined relevant macros.
# predefinedMacros_SOURCES = predefinedMacros.C

if ROSE_WITH_ATERM
# The Stratego and A-Term support is outside of ROSE
# This needs a configure flag so that the location outside 
# of ROSE can be easily specificed at configure time.
# ATERM_INCLUDE = -I$(ATERM_INSTALL_PATH)/include
# ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib
# ATERM_INCLUDE = $(ROSE_ATERM_INCLUDE)
# ATERM_LIB_DIR = $(ROSE_ATERM_LIBS)
endif

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
# AM_CPPFLAGS = $(ROSE_INCLUDES) $(ATERM_INCLUDE)
AM_CPPFLAGS = $(ROSE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) --Wl,--no-undefined
# AM_LDFLAGS  = $(ROSE_RPATHS)
# AM_LDFLAGS  = $(ROSE_RPATHS) --Wl,--no-undefined

# DQ (11/15/2017): Mac OSX can't take the -Wl,--no-undefined as a linker option.
# AM_LDFLAGS  = $(ROSE_RPATHS) -Wl,--no-undefined
AM_LDFLAGS  = $(ROSE_RPATHS)

LDADD       = $(ROSE_LIBS)
# LDADD       = $(ROSE_LIBS) --Wl,--no-undefined
# LDADD       = $(ROSE_LIBS) --no-undefined

testAtermGraphGeneration_LDADD = $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt input_codes inputFile.C tests.tex

# ********************************
# *******  Analysis Tests  *******
# ********************************
testObjectFileAnalysis: testAnalysis
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	DQ (4/19/2011): copy $(srcdir)/inputFile.C to intermediate file to avoid collisions in generation of rose_inputFile.C within parallel make.
	cp $(srcdir)/inputFile.C inputObjectFileAnalysis.C
#	./testAnalysis $(AM_CPPFLAGS) -c $(srcdir)/inputFile.C -o alt_Analysis_inputFile.o
#	./testAnalysis $(AM_CPPFLAGS) -c inputObjectFileAnalysis.C -o alt_Analysis_inputFile.o
	./testAnalysis -c inputObjectFileAnalysis.C -o alt_Analysis_inputFile.o
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testLinkFileAnalysis: testObjectFileAnalysis
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	./testAnalysis $(AM_CPPFLAGS) alt_inputFile.o -o alt_inputFile
#	./testAnalysis $(AM_CPPFLAGS) alt_Analysis_inputFile.o -o testLinkFileAnalysisExecutable
	./testAnalysis alt_Analysis_inputFile.o -o testLinkFileAnalysisExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testCppFileAnalysis: testAnalysis
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	./testAnalysis -E $(AM_CPPFLAGS) $(srcdir)/inputFile.C
	./testAnalysis -E $(srcdir)/inputFile.C
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testExecutableFileAnalysis: testAnalysis
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	cp $(srcdir)/inputFile.C inputFileAnalysis.C
#	./testAnalysis $(AM_CPPFLAGS) inputFileAnalysis.C -o testExecutableFileAnalysisExecutable
	./testAnalysis inputFileAnalysis.C -o testExecutableFileAnalysisExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

test_testAnalysis: testObjectFileAnalysis testLinkFileAnalysis testCppFileAnalysis testExecutableFileAnalysis


# ****************************************
# *******  Code Generation Tests  ********
# ****************************************
testObjectFileCodeGeneration: testCodeGeneration
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	DQ (4/19/2011): copy $(srcdir)/inputFile.C to intermediate file to avoid collisions in generation of rose_inputFile.C within parallel make.
	cp $(srcdir)/inputFile.C inputObjectFileCodeGeneration.C
#	./testCodeGeneration $(AM_CPPFLAGS) -c $(srcdir)/inputFile.C -o alt_CodeGeneration_inputFile.o
#	./testCodeGeneration $(AM_CPPFLAGS) -c inputObjectFileCodeGeneration.C -o alt_CodeGeneration_inputFile.o
	./testCodeGeneration -c inputObjectFileCodeGeneration.C -o alt_CodeGeneration_inputFile.o
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testLinkFileCodeGeneration: testObjectFileCodeGeneration
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	./testCodeGeneration -rose:Cxx alt_CodeGeneration_inputFile.o -o testLinkFileCodeGenerationExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testCppFileCodeGeneration: testCodeGeneration
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	./testCodeGeneration -E $(AM_CPPFLAGS) $(srcdir)/inputFile.C
	./testCodeGeneration -E $(srcdir)/inputFile.C
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testExecutableFileCodeGeneration: testCodeGeneration
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	cp $(srcdir)/inputFile.C inputFileCodeGeneration.C
#	./testCodeGeneration $(AM_CPPFLAGS) inputFileCodeGeneration.C -o testExecutableFileCodeGenerationExecutable
	./testCodeGeneration inputFileCodeGeneration.C -o testExecutableFileCodeGenerationExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

test_testCodeGeneration: testObjectFileCodeGeneration testLinkFileCodeGeneration testCppFileCodeGeneration testExecutableFileCodeGeneration


# **********************************
# *******  Translator Tests  *******
# **********************************
testObjectFileTranslator: testTranslator
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	DQ (4/19/2011): copy $(srcdir)/inputFile.C to intermediate file to avoid collisions in generation of rose_inputFile.C within parallel make.
	cp $(srcdir)/inputFile.C inputObjectFileTranslator.C
#	./testTranslator $(AM_CPPFLAGS) -c $(srcdir)/inputFile.C -o alt_Translator_inputFile.o
#	./testTranslator $(AM_CPPFLAGS) -c inputObjectFileTranslator.C -o alt_Translator_inputFile.o
	./testTranslator -c inputObjectFileTranslator.C -o alt_Translator_inputFile.o
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testLinkFileTranslator: testObjectFileTranslator
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
#	./testTranslator $(AM_CPPFLAGS) alt_Translator_inputFile.o -o testLinkFileTranslatorExecutable
	./testTranslator alt_Translator_inputFile.o -o testLinkFileTranslatorExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testCppFileTranslator: testTranslator
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	./testTranslator -E $(AM_CPPFLAGS) $(srcdir)/inputFile.C
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

testExecutableFileTranslator: testTranslator
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	cp $(srcdir)/inputFile.C inputFileTranslator.C
#	./testTranslator $(AM_CPPFLAGS) inputFileTranslator.C -o testExecutableFileTranslatorExecutable
	./testTranslator inputFileTranslator.C -o testExecutableFileTranslatorExecutable
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

test_testTranslator: testObjectFileTranslator testLinkFileTranslator testCppFileTranslator testExecutableFileTranslator

# Reported failure from Tristan
testSimpleLinkFileTranslator: testTranslator
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	cp $(srcdir)/inputFile.C inputSimpleLinkFileTranslator.C
	./testTranslator inputSimpleLinkFileTranslator.C -lm
else
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif


# *****************************************
# *******  Token Generation Tests  ********
# *****************************************
testObjectFileTokenGeneration: testTokenGeneration
#	DQ (4/19/2011): copy $(srcdir)/inputFile.C to intermediate file to avoid collisions in generation of rose_inputFile.C within parallel make.
	cp $(srcdir)/inputFile.C alt_ObjectFileTokenGeneration_inputFile.C
#	./testTokenGeneration $(AM_CPPFLAGS) -c $(srcdir)/inputFile.C -o alt_TokenGeneration_inputFile.o
#	./testTokenGeneration $(AM_CPPFLAGS) -c alt_ObjectFileTokenGeneration_inputFile.C -o alt_TokenGeneration_inputFile.o
	./testTokenGeneration -c alt_ObjectFileTokenGeneration_inputFile.C -o alt_TokenGeneration_inputFile.o

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
test_testTokenGeneration: testObjectFileTokenGeneration
else
test_testTokenGeneration:
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

if !ROSE_USE_CLANG_FRONTEND

test_testGraphGeneration: testGraphGeneration

if ROSE_WITH_LIBHARU
test_testPDFGeneration: testPDFGeneration
endif

# DQ (74/6/2017): Adding test for the version of identityTranslator that is distributed 
# in the exampleTranslators/documentedExamples/simpleTranslatorExamples directory.
exampleIdentityTranslator:
	$(MAKE) -C $(top_builddir)/exampleTranslators/documentedExamples/simpleTranslatorExamples

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
BAD_PATH = $(builddir)/XXX
test_exampleIdentityTranslator: exampleIdentityTranslator
	cp $(srcdir)/inputFile.C alt_identityTranslator_inputFile.C
	@echo "Test the identityTranslator distributed in $(top_builddir)/exampleTranslators/documentedExamples/simpleTranslatorExamples"
	$(top_builddir)/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator -c alt_identityTranslator_inputFile.C
	@echo "Test the identityTranslator using a bad path with the -I option (previous failing case)."
	$(top_builddir)/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator -I$(BAD_PATH) -c alt_identityTranslator_inputFile.C
else
test_exampleIdentityTranslator:
	@echo "SKIPPING target '$@' because the C/C++ frontend is not enabled."
endif

# Rule to run all the example translator tests
# test: test_testAnalysis test_testCodeGeneration test_testTranslator testSimpleLinkFileTranslator
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
if ROSE_WITH_LIBHARU
   test: test_testTokenGeneration test_testAnalysis test_testCodeGeneration test_testTranslator test_testTemplates test_testGraphGeneration test_testPDFGeneration testSimpleLinkFileTranslator testTranslatorFoldedConstants testTranslatorUnfoldedConstants testKeepGoingTranslator test_readFileTwice
else
   test: test_testTokenGeneration test_testAnalysis test_testCodeGeneration test_testTranslator test_testTemplates test_testGraphGeneration testSimpleLinkFileTranslator testTranslatorFoldedConstants testTranslatorUnfoldedConstants testKeepGoingTranslator test_readFileTwice
endif
else
if ROSE_WITH_LIBHARU
   test: test_testTokenGeneration test_testAnalysis test_testCodeGeneration test_testTranslator test_testTemplates test_testGraphGeneration test_testPDFGeneration testSimpleLinkFileTranslator testTranslatorFoldedConstants testTranslatorUnfoldedConstants testKeepGoingTranslator test_exampleIdentityTranslator test_readFileTwice
else
   test: test_testTokenGeneration test_testAnalysis test_testCodeGeneration test_testTranslator test_testTemplates test_testGraphGeneration testSimpleLinkFileTranslator testTranslatorFoldedConstants testTranslatorUnfoldedConstants testKeepGoingTranslator test_exampleIdentityTranslator test_readFileTwice
endif
endif

# Liao, 4/12/2017 restrict_template_parameter.C will trigger frontend error.
# __minimal_input_code is defined in input_codes/Makefile.for.input.codes
testKeepGoingTranslator: $(top_builddir)/exampleTranslators/defaultTranslator/KeepGoingTranslator
	./$(top_builddir)/exampleTranslators/defaultTranslator/KeepGoingTranslator -c $(__minimal_input_code) $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests/restrict_template_parameter.C $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests/test2017_13_Liao.C $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests/test2017_12_Liao.C

# DQ (7/2/2015): template test code that marks all template instantiations for output (as a test).
test_testTemplates: testTemplates

# DQ (7/12/2019): Adding support for testing outliner feature to outpline functions to a seperate file.
test_readFileTwice: readFileTwice

# This appears to be a bug, in that the executable is not build (ROSE assumes an implicit -c for compile only mode).
#predefinedMacros: testTranslator
#	./testTranslator $(srcdir)/predefinedMacros.C

#testPredefinedMacros: predefinedMacros
#	./predefinedMacros

# Make check rule
conditional-check-local: test
	@echo "************************************************************************"
	@echo "****** ROSE/tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************"

else

conditional-check-local:

endif

check-local: conditional-check-local

clean-local: myCleanRule

myCleanRule:
	rm -f testLinkFileAnalysisExecutable testLinkFileCodeGenerationExecutable testLinkFileTranslatorExecutable 
	rm -f testExecutableFileAnalysisExecutable testExecutableFileCodeGenerationExecutable testExecutableFileTranslatorExecutable
	rm -f rose_*.C inputFileTranslator.C inputFileCodeGeneration.C inputFileAnalysis.C inputSimpleLinkFileTranslator.C
	rm -f inputObjectFileAnalysis.C inputObjectFileCodeGeneration.C inputObjectFileTranslator.C alt_ObjectFileTokenGeneration_inputFile.C
	rm -f a.out *.dot *.binary
	rm -f *.C_identity
	rm -rf rose_minimal.cpp rose-passed_files.txt rose-failed_files.txt

