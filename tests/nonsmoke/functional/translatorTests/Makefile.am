include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

# roseCompiler_DEPENDENCIES = $(REQUIRED_LIBS)
# roseCompiler_LDADD  = $(ROSE_LIBS)
DEPENDENCIES = $(REQUIRED_LIBS)
LDADD  = $(ROSE_LIBS)


# DQ (2/28/2017): NOTE: bug125 is executed and takes a very long time (about 15 minutes; we should maybe look into this).

# most test translators should not be installed (noinst)
# The bug numbers come from ROSE's SciDAC bug trackers
#github_83_outline, this test uses C++11, we have better regression test for this bug. Disable it for now.
noinst_PROGRAMS = \
	bug125 \
	qualifiedName \
	testTranslator2010_01 \
	testTranslator2010_02 \
	testTranslator2010_03 \
	testTranslator2012_01 \
	ompVariableCollecting \
	label_stmt_file_info_translator \
	testDataMemberOfConst \
	testFortranParameter \
	testFortranProtected \
	label_stmt_file_info_translator \
	testConstDeclarations \
	testPragmaInBody \
	querySubTree \
	testCopyAndDelete \
        testTraversalOfTemplateInstantiations \
	testTranslator2013_01 \
	testTranslator2013_02 \
	testTranslator2018_01 \
	testTranslator2018_02 \
	testPlugins
#
# a better alternative to identityTranslator
# Liao 1/29/2009
bin_PROGRAMS = roseCompiler
roseCompiler_SOURCES = roseCompiler.C
testPlugins_SOURCES = testPlugins.C

# Example translators used in regression tests
bug125_SOURCES = bug125.C
qualifiedName_SOURCES = qualifiedName.C
testDataMemberOfConst_SOURCES = testDataMemberOfConst.C
testFortranParameter_SOURCES = testFortranParameter.C
testFortranProtected_SOURCES = testFortranProtected.C

testTranslator2010_01_SOURCES = testTranslator2010_01.C
testTranslator2010_02_SOURCES = testTranslator2010_02.C
testTranslator2010_03_SOURCES = testTranslator2010_03.C
ompVariableCollecting_SOURCES = ompVariableCollecting.C
testTranslator2012_01_SOURCES = testTranslator2012_01.C

label_stmt_file_info_translator_SOURCES = label_stmt_file_info_translator.C

# DQ (8/25/2011): Added test code to demonstrate where const is set in the AST.
testConstDeclarations_SOURCES = testConstDeclarations.C
testPragmaInBody_SOURCES = testPragmaInBody.C

querySubTree_SOURCES = querySubTree.C

testCopyAndDelete_SOURCES = testCopyAndDelete.C

testTraversalOfTemplateInstantiations_SOURCES = testTraversalOfTemplateInstantiations.C

testTranslator2013_01_SOURCES = testTranslator2013_01.C
testTranslator2013_02_SOURCES = testTranslator2013_02.C
testTranslator2018_01_SOURCES = testTranslator2018_01.C
testTranslator2018_02_SOURCES = testTranslator2018_02.C
#github_83_outline_SOURCES = github_83_outline.C
# LDADD = $(ROSE_SEPARATE_LIBS)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes

VALGRIND =

# DQ (11/11/2012): This tests the PDF generater on a large program (includes STL string).
test125.pdf: bug125 
	./bug125 -c $(srcdir)/inputbug125.C

# testqualifiedName: qualifiedName

jacobi.o: qualifiedName
	./qualifiedName -c $(top_srcdir)/projects/autoTuning/tests/jacobi.c

dataMemberOfConst.o: testDataMemberOfConst
	./testDataMemberOfConst $(srcdir)/../CompileTests/Cxx_tests/dataMemberOfConst.C -o /dev/null && touch $@

input_ompVariableCollecting.o: ompVariableCollecting
	./ompVariableCollecting -I$(top_srcdir)/src/frontend/SageIII -rose:OpenMP:ast_only -c $(srcdir)/input_ompVariableCollecting.C && touch $@

input_testConstDeclarations.o: testConstDeclarations
	./testConstDeclarations $(srcdir)/input_testConstDeclarations.C && touch $@

check_PROGRAMS = testTranslator2010_01
TESTS = $(check_PROGRAMS) #test_testDataMemberOfConst test_testFortranParameter

# We have to touch input_testPragmaInBody.o since the translator does not invoke backend at all
input_testPragmaInBody.o:testPragmaInBody
	./testPragmaInBody -c $(srcdir)/input_testPragmaInBody.c  && touch $@

input_querySubTree.o:querySubTree
	./querySubTree -c $(srcdir)/input_querySubTree.C  && touch $@

input_testTranslator2010_2.o: testTranslator2010_02 
	./testTranslator2010_02 -rose:verbose 0 $(srcdir)/input_testTranslator2010_2.C && touch $@

input_testTranslator2010_3.o: testTranslator2010_03 
	./testTranslator2010_03 -c -rose:verbose 0 $(srcdir)/input_testTranslator2010_3.C && touch $@

# DQ (2/4/2011): Added test code to evaluate line numbers of SgLabelStatement that
# were previously marked as transforamtions and thus has line numbers == zero.
input_label_stmt_file_info_translator.o: label_stmt_file_info_translator
	./label_stmt_file_info_translator -rose:verbose 0 -c $(srcdir)/input_label_stmt_file_info_translator.C && touch $@

input_testTranslator2012_01b.o: testTranslator2012_01
	./testTranslator2012_01 -rose:verbose 0 $(srcdir)/input_testTranslator2012_01a.C $(srcdir)/input_testTranslator2012_01b.C && touch $@


lib_LTLIBRARIES = libPrintNamesPlugin.la
libPrintNamesPlugin_la_SOURCES = PrintNamesPlugin.cpp



#PLUGIN_OPTS=-rose:plugin_lib lib.so -rose:plugin_lib lib2.so -rose:plugin_action act1 -rose:plugin_action act2 -rose:plugin_arg_act1 op1 -rose:plugin_arg_act1 op2 -rose:plugin_arg_act2 op3 -rose:plugin_arg_act2 op4
PLUGIN_OPTS=-rose:plugin_lib .libs/libPrintNamesPlugin.so -rose:plugin_action print-names -rose:plugin_arg_print-names op1
input_testPlugins.o: testPlugins libPrintNamesPlugin.la
	./testPlugins $(PLUGIN_OPTS) -c $(srcdir)/input_testPlugins.C && touch $@


# Fortran tests
parameter.o: testFortranParameter
	./testFortranParameter $(srcdir)/../CompileTests/Fortran_tests/parameter.f -o /dev/null && touch $@

protected.o: testFortranProtected
	gfortran -c $(srcdir)/../CompileTests/Fortran_tests/protected_mod.f03
	gdb --args .libs/testFortranProtected $(srcdir)/../CompileTests/Fortran_tests/protected.f03 -o /dev/null && touch $@

input_testCopyAndDelete.o:
	./testCopyAndDelete -c $(srcdir)/input_testCopyAndDelete.c  && touch $@

input_testCopyAndDelete_2.o:
	./testCopyAndDelete -c $(srcdir)/input_testCopyAndDelete_2.C  && touch $@

input_testTraversalOfTemplateInstantiations.o:
	./testTraversalOfTemplateInstantiations -c $(srcdir)/input_testTraversalOfTemplateInstantiations.C  && touch $@

input_testTranslator2013_01.o: testTranslator2013_01
	./testTranslator2013_01 -rose:verbose 0 $(srcdir)/input_testTranslator2013_01.C && touch $@

input_testTranslator2013_02.o: testTranslator2013_02
	./testTranslator2013_02 -rose:verbose 0 $(srcdir)/input_testTranslator2013_02.C && touch $@

input_testTranslator2018_01.o: testTranslator2018_01
	./testTranslator2018_01 -DSKIP_ROSE_BUILTIN_DECLARATIONS -rose:verbose 0 -c $(srcdir)/input_testTranslator2018_01.C && touch $@

input_testTranslator2018_02.o: testTranslator2018_02
	./testTranslator2018_02 -rose:verbose 0 -c $(srcdir)/input_testTranslator2018_02.C && touch $@

#input_github_83_outline.o: github_83_outline
#	./github_83_outline -rose:verbose 0  --rose:openmp:ast_only -c $(srcdir)/input_github_83_outline.C && touch $@

EXTRA_DIST = \
   CMakeLists.txt \
   inputbug125.C \
   input_testTranslator2010_2.C \
   input_testTranslator2010_3.C \
   input_ompVariableCollecting.C \
   input_label_stmt_file_info_translator.C \
   input_testTranslator2012_01a.C \
   input_testTranslator2012_01a.C \
   input_testPragmaInBody.c \
   input_querySubTree.C \
   input_github_83_outline.C

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
# input_github_83_outline.o, 2/18/2020
check-cxx: \
	test125.pdf \
	jacobi.o \
	input_testPragmaInBody.o \
	input_ompVariableCollecting.o \
	input_testConstDeclarations.o \
	input_testTranslator2010_2.o \
	input_testTranslator2010_3.o \
	input_label_stmt_file_info_translator.o \
	input_querySubTree.o 
# TODO: FIX the following tests
#Currently_Failing_Cxx_tests =
#	dataMemberOfConst.o 

else
check-cxx:
endif

# DQ (3/3/2017): Note that this directory of tests are only run for C/C++ langauge support is enabled.
# Liao, 1/13/2014, these two tests are not tested by EDG 3 based ROSE, comment out for now
#	parameter.o 
#	protected.o
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-fortran:
else
check-fortran:
endif

check-local:
#	@$(MAKE) check-cxx check-fortran
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
	@echo "OpenMP tests using EDG 4.9 and GNU version 4.9 backend compiler are disabled."
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
	@$(MAKE) check-cxx check-fortran
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "Translator tests using EDG 4.9: Clang backend compiler are NOT allowed with the EDG 4.9 frontend."
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "Translator tests using EDG 4.9 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "Translator tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "Translator tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) check-cxx check-fortran
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "Translator tests using EDG 4.9: Clang backend compiler using EDG 4.12 frontend."
	@$(MAKE) check-cxx check-fortran
# DQ (2/28/2017): Skip these files with Clang for now and come back to them later (when we understand the limits of portability better).
#	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "Translator tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) check-cxx check-fortran
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/translatorTests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************************"


distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C *.dot *.cpp

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o

