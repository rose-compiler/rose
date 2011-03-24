include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# See the README file (in this dirctory) for a brief description of the purpuse for each subdirectory.
# SUBDIRS = A++Code 
SUBDIRS = \
   testOutputFileOption \
   testHeaderFileOutput testFileNamesAndExtensions testGnuOptions \
   testWave \
   preinclude_tests tokenStream_tests

# TOO (2/16/2011): Errors with Thrifty (GCC 3.4.4) backend compiler. We can gradually
# enable these tests at a later stage if necessary.
if !USING_GCC_3_4_4_BACKEND_COMPILER
  SUBDIRS += testCpreprocessorOption 
if !USING_XTENSA_BACKEND_COMPILER
  SUBDIRS += testGenerateSourceFileNames 
endif
endif

# TOO (2/23/2011): Errors with Tensilica's Xtensa compilers as alternative backend compilers. We can
# gradually enable these tests at a later stage if necessary.
if !USING_XTENSA_BACKEND_COMPILER
  SUBDIRS += testForSpuriousOutput collectAllCommentsAndDirectives_tests 
endif

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionsTests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************"
