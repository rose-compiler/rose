include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (2/6/2010): Reordered a few of the first directories to have the most relevant test run first by "make check"
# This should make the testing of ROSE more meaningful for users.
# SUBDIRS = \
#      C_tests Cxx_tests C_subset_of_Cxx_tests C99_tests Fortran_tests UPC_tests \
#      copyAST_tests RoseExample_tests PythonExample_tests \
#      colorAST_tests mergeAST_tests unparseToString_tests \
#      sourcePosition_tests OpenMP_tests \
#      boost_tests virtualCFG_tests uninitializedField_tests \
#      A++Tests OvertureCode P++Tests A++Code ElsaTestCases \
#      ExpressionTemplateExample_tests hiddenTypeAndDeclarationListTests \
#      sizeofOperation_tests MicrosoftWindows_tests nameQualificationAndTypeElaboration_tests \
#      UnparseHeadersTests

# DQ (12/5/2007): Removed A++Code because this takes a long time (about 1 minute).


# DQ (4/17/2010): These directories are now organized based on what sets of languages 
# are enabled in ROSE (including binary analysis).

# DQ (7/28/2014): Moved this to aver the C and C++ language tests.
# SUBDIRS = x10_tests
SUBDIRS = 

if ROSE_USE_CLANG_FRONTEND
   SUBDIRS += C_tests
   SUBDIRS += C89_std_c89_tests
   SUBDIRS += C_subset_of_Cxx_tests
   SUBDIRS += CudaTests
   SUBDIRS += OpenClTests
else
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += C_tests Cxx_tests C_subset_of_Cxx_tests C99_tests C11_tests Cxx11_tests UPC_tests MicrosoftWindows_C_tests MicrosoftWindows_Cxx_tests
endif
endif

# DQ (7/28/2014): Moved to after the C and C++ langauge tests.
SUBDIRS += x10_tests

SUBDIRS += OpenMP_tests FailSafe_tests

# DQ (7/25/2010): These should not be at the end, but it is easier to split out Fortran support this way.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
# DQ (7/14/2013): Temporarily commented out this test to evaluate progress on tests/CompileTests directory.
#  SUBDIRS += Fortran_tests CAF2_tests
   SUBDIRS += Fortran_tests
#Pei-Hung (7/13/2015): Adding mixlanguage tests (patches from Stephen Guzik)
   SUBDIRS += mixLanguage_tests
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += Java_tests MicrosoftWindows_Java_tests
endif

# DQ (10/19/2010): Moved basic languge test to the front of the list.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += \
      copyAST_tests RoseExample_tests PythonExample_tests \
      colorAST_tests unparseToString_tests \
      sourcePosition_tests \
      boost_tests uninitializedField_tests \
      OvertureCode P++Tests A++Code \
      ExpressionTemplateExample_tests hiddenTypeAndDeclarationListTests \
      sizeofOperation_tests MicrosoftWindows_tests nameQualificationAndTypeElaboration_tests \
      UnparseHeadersTests

# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += mergeAST_tests ElsaTestCases virtualCFG_tests A++Tests staticCFG_tests 
endif
endif

# end of !ROSE_USE_CLANG_FRONTEND
endif

if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
   SUBDIRS += Python_tests
endif

SUBDIRS += CudaTests

# Used to check all C and C++ test codes: not really specific to EDG 4.x
SUBDIRS += frontend_integration

# DQ (2/15/2014): Adding systemc tests (part of general C++ robustness tests)
SUBDIRS += systemc_tests

# end of ROSE_USE_CLANG_FRONTEND
# endif

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/tests/CompileTests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"

