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
else # NOT ROSE_USE_CLANG_FRONTEND
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
# DQ (2/18/2017): We can't use the EDG 4.9 frontend with later versions of compiler because it can't parse a few of the header files from those compilers.
# LLVM header files are actually a copy of the GNU header files (e.g. LLVM 3.5 uses GNU 4.9 headers).
SUBDIRS += C_tests Cxx_tests C_subset_of_Cxx_tests C99_tests C11_tests Cxx11_tests Cxx14_tests UPC_tests MicrosoftWindows_C_tests MicrosoftWindows_Cxx_tests

# DQ (1/1/2019): Only EDG 5.0 is has sufficent support for C++17 (I think).
if ROSE_USE_EDG_VERSION_5_0
SUBDIRS += Cxx17_tests
if ROSE_FRONTEND_COMPILER_DEFAULT_IS_CXX11
if ROSE_USING_GCC_VERSION_LATER_6_1
SUBDIRS += unparse_template_from_ast
endif
endif
endif

endif
endif

# DQ (7/28/2014): Moved to after the C and C++ langauge tests.
SUBDIRS += x10_tests


if !ROSE_USE_SANITIZER
# DQ (7/25/2010): These should not be at the end, but it is easier to split out Fortran support this way.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
# DQ (7/14/2013): Temporarily commented out this test to evaluate progress on tests/nonsmoke/functional/CompileTests directory.
#  SUBDIRS += Fortran_tests CAF2_tests
   SUBDIRS += Fortran_tests
#Pei-Hung (7/13/2015): Adding mixlanguage tests (patches from Stephen Guzik)
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += mixLanguage_tests
endif
endif
endif

# Liao 2/130/2017: moved to after Fortran tests. So OpenMP's Fortran tests will be tested after Fortran_tests
# DQ (1/15/2017): Added conditional support to avoid these tests when ROSE is configured for binary only support, failing in ROSE matrix testing).
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

# DQ (1/15/2017): Clang does not support omp.h include file required for OpenMP testing.
# And we don't have a AUTOMAKE conditional defined for the use of Clang as the backend compiler.
# if !USING_CLANG_COMPILER
# SUBDIRS += OpenMP_tests FailSafe_tests
# endif


if USING_GNU_COMPILER
SUBDIRS += OpenMP_tests OpenACC_tests FailSafe_tests
else # NOT USING_GNU_COMPILER
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
else # NOT USING_CLANG_COMPILER
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
SUBDIRS += OpenMP_tests OpenACC_tests FailSafe_tests
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
if !ROSE_USE_SANITIZER
#Switch Java_tests to seperate run due to issues with large number of processors
   SUBDIRS += MicrosoftWindows_Java_tests
endif
endif

# DQ (10/19/2010): Moved basic languge test to the front of the list.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += \
      copyAST_tests PythonExample_tests \
      colorAST_tests unparseToString_tests \
      sourcePosition_tests \
      boost_tests uninitializedField_tests \
      OvertureCode P++Tests A++Code \
      ExpressionTemplateExample_tests hiddenTypeAndDeclarationListTests \
      sizeofOperation_tests MicrosoftWindows_tests nameQualificationAndTypeElaboration_tests \
      UnparseHeadersTests UnparseHeadersUsingTokenStream_tests

# DQ (11/5/2016): These tests fail when testing with Address Sanitizer, but I can't see the output. 
# Need to work work with Markus to understand the results better, skip these tests for now is using
# Address Sanitizer.
if !ROSE_USE_SANITIZER
# DQ (7/2/2020): GNU 6.1 and later will fail for these tests.
if !ROSE_USING_GCC_VERSION_LATER_6_1
   SUBDIRS += STL_tests
endif
endif

# TOO (2/16/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += mergeAST_tests ElsaTestCases A++Tests staticCFG_tests 
endif

# DQ (11/5/2016): The Fortran tests in this directory fail (issues with JVM and too much parallelisn 
# (-j24) which running with address sanitizer).  It is likely that we can skip only the Fortran
# tests in this directory as a better alternative.
if !ROSE_USE_SANITIZER
   SUBDIRS += virtualCFG_tests
endif

# DQ (11/5/2016): Moved this to the end so that we can evaluate other testing results before this one.
# Also skip these tests when using the address sanitizer since they would take SO LONG (at least for 
# initial testing, I think).
if !ROSE_USE_SANITIZER
SUBDIRS += RoseExample_tests 
endif

endif # ROSE_BUILD_CXX_LANGUAGE_SUPPORT

# end of !ROSE_USE_CLANG_FRONTEND
endif

if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
   SUBDIRS += Python_tests
endif

# DQ (1/15/2017): Added conditional support to avoid these tests when ROSE is configured for binary only support, failing in ROSE matrix testing).
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
SUBDIRS += CudaTests

# Used to check all C and C++ test codes: not really specific to EDG 4.x
SUBDIRS += frontend_integration
endif

# DQ (2/15/2014): Adding systemc tests (part of general C++ robustness tests)
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
SUBDIRS += systemc_tests

if ROSE_USE_VXWORKS
# DQ (5/21/2017): Adding VX Works support.
SUBDIRS += vxworks_tests
endif
endif

# DQ (8/23/2017): Adding support for testing experimental Charp support.
if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
SUBDIRS += experimental_csharp_tests
endif

# DQ (8/23/2017): Adding support for testing experimental Ada support.
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
SUBDIRS += experimental_ada_tests
endif

# DQ (8/23/2017): Adding support for testing experimental Jovial support.
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
SUBDIRS += experimental_jovial_tests
endif

# DQ (8/23/2017): Adding support for testing experimental Cobol support.
if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
SUBDIRS += experimental_cobol_tests
endif

# Rasmussen (11/4/2017): Adding support for testing experimental Matlab support.
if ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION
SUBDIRS += experimental_matlab_tests
endif

if ROSE_USE_GNAT
# DQ (11/14/2017): Removed GNAT test directory since it is redundant with the ADA test directory.
# DQ (6/8/2017): Adding GNAT support.
# SUBDIRS += gnat_tests
endif

# end of ROSE_USE_CLANG_FRONTEND
endif

# DQ (31/8/2017): Add test specific to just the newer experimental language support.
test_new_languages:
#	cd experimental_csharp_tests; $(MAKE) test
	cd experimental_ada_tests;    $(MAKE) test
	cd experimental_jovial_tests; $(MAKE) test
	cd experimental_cobol_tests;  $(MAKE) test

# TV (08/07/2018): Added `status` target which generate `status.csv`
status:
	@$(srcdir)/status.sh compiler
	@$(srcdir)/status.sh backend
	@$(srcdir)/status.sh translator


