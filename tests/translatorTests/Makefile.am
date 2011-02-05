include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

# an better alternative to identityTranslator
# Liao 1/29/2009
bin_PROGRAMS = roseCompiler testDataMemberOfConst testFortranParameter testFortranProtected label_stmt_file_info_translator

roseCompiler_SOURCES = roseCompiler.C

# roseCompiler_DEPENDENCIES = $(REQUIRED_LIBS)
# roseCompiler_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
DEPENDENCIES = $(REQUIRED_LIBS)
LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# The bug numbers come from ROSE's SciDAC bug trackers
noinst_PROGRAMS = \
   bug125 \
   qualifiedName \
   testTranslator2010_01 \
   testTranslator2010_02 \
   testTranslator2010_03 \
   ompVariableCollecting \
   label_stmt_file_info_translator

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

label_stmt_file_info_translator_SOURCES = label_stmt_file_info_translator.C

# LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes

VALGRIND =

testbug125: bug125 
	./bug125 -c $(srcdir)/inputbug125.C

testqualifiedName: qualifiedName
	./qualifiedName -c $(top_srcdir)/projects/autoTuning/tests/jacobi.c

test_testDataMemberOfConst: testDataMemberOfConst
	./testDataMemberOfConst $(srcdir)/../CompileTests/Cxx_tests/dataMemberOfConst.C -o /dev/null

test_testFortranParameter: testFortranParameter
	./testFortranParameter $(srcdir)/CompileTests/Fortran_tests/parameter.f -o /dev/null

test_testFortranProtected: testFortranProtected
	gfortran -c $(srcdir)/../CompileTests/Fortran_tests/protected_mod.f03
	gdb --args .libs/testFortranProtected $(srcdir)/../CompileTests/Fortran_tests/protected.f03 -o /dev/null

test_ompVariableCollecting: ompVariableCollecting
	./ompVariableCollecting -I$(top_srcdir)/src/frontend/SageIII -rose:OpenMP:ast_only -c $(srcdir)/input_ompVariableCollecting.C

#testTranslator2010_01_test: testTranslator2010_01 
#	./testTranslator2010_01 -rose:verbose 2
# a few translators without input parameters to run	
check_PROGRAMS = testTranslator2010_01
TESTS = $(check_PROGRAMS) #test_testDataMemberOfConst test_testFortranParameter

testTranslator2010_02_test: testTranslator2010_02 
	./testTranslator2010_02 -rose:verbose 0 $(srcdir)/input_testTranslator2010_2.C

testTranslator2010_03_test: testTranslator2010_03 
	./testTranslator2010_03 -rose:verbose 0 $(srcdir)/input_testTranslator2010_3.C

test_label_stmt_file_info_translator: label_stmt_file_info_translator
	./label_stmt_file_info_translator -rose:verbose 0 -c $(srcdir)/input_label_stmt_file_info_translator.C

EXTRA_DIST = CMakeLists.txt inputbug125.C input_testTranslator2010_2.C input_testTranslator2010_3.C input_ompVariableCollecting.C

check-local:
#	@$(MAKE) testbug125 testqualifiedName testTranslator2010_01_test
	@$(MAKE) testbug125 testqualifiedName test_ompVariableCollecting
	@$(MAKE) testTranslator2010_02_test
	@echo "****************************************************************************************"
	@echo "****** ROSE/tests/translatorTests: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************"


distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C *.dot *.cpp

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o

