include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
#	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == true"
# DQ (3/17/2017): Make sure this is tested for the GNU 4.8 compiler (previously only GNU versions less than 4.8).
if !ROSE_USING_GCC_VERSION_LATER_4_9
#	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
SUBDIRS = \
   testOutputFileOption \
   testHeaderFileOutput testFileNamesAndExtensions testGnuOptions \
   testWave \
   preinclude_tests tokenStream_tests testIncludeOptions

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
endif # ROSE_USING_GCC_VERSION_LATER_4_9
else  # NOT USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == false"
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
if USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == true"
# DQ (3/17/2017): Make sure this is tested for the GNU 4.8 compiler (previously only GNU versions less than 4.8).
if !ROSE_USING_GCC_VERSION_LATER_4_9
#	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
SUBDIRS = \
   testOutputFileOption \
   testHeaderFileOutput testFileNamesAndExtensions testGnuOptions \
   testWave \
   preinclude_tests tokenStream_tests testIncludeOptions

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
endif # ROSE_USING_GCC_VERSION_LATER_4_9
else  # NOT USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == false"
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9

# See the README file (in this dirctory) for a brief description of the purpuse for each subdirectory.
# SUBDIRS = A++Code 

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompilerOptionsTests: make check rule complete (terminated normally) *****"
	@echo "********************************************************************************************"
