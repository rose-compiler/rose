include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

# roseCompiler_DEPENDENCIES = $(REQUIRED_LIBS)
# roseCompiler_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
DEPENDENCIES = $(REQUIRED_LIBS)
LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# most test translators should not be installed (noinst)
# The bug numbers come from ROSE's SciDAC bug trackers
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
	testTranslator2013_02
#
# a better alternative to identityTranslator
# Liao 1/29/2009
bin_PROGRAMS = roseCompiler
roseCompiler_SOURCES = roseCompiler.C


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

# LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

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
   input_querySubTree.C

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
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

# Liao, 1/13/2014, these two tests are not tested by EDG 3 based ROSE, comment out for now
#	parameter.o 
#	protected.o
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-fortran:
else
check-fortran:
endif

check-local:
	@$(MAKE) check-cxx check-fortran
	@echo "****************************************************************************************"
	@echo "****** ROSE/tests/translatorTests: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************"


distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C *.dot *.cpp

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o

