include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (4/17/2010): These directories are now organized based on what sets of languages 
# are enabled in ROSE (including binary analysis).
SUBDIRS =

# DQ (9/18/2020): Comment out as a test so that we can use clang and Intel compilers.
# if USING_GNU_COMPILER

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

   SUBDIRS += astMergeTests astPerformanceTests \
              astProcessingTests astQueryTests astRewriteTests astSymbolTableTests astTokenStreamTests astSnippetTests \
              programTransformationTests \
              graph_tests mergeTraversal_tests \
              astLValueTests abstractMemoryObjectTests varDeclNorm

# DQ (11/5/2016): Skip these tests when using the address sanitizer since they would 
# take SO LONG (at least over an hour for initial testing, I think).
if !ROSE_USE_SANITIZER
   SUBDIRS += ROSETTA
endif

if !ROSE_USE_INSURE
   SUBDIRS += roseHPCToolkitTests 
endif

# TOO (2/16/2011): Errors with Thrifty (GCC 3.4.4) backend compiler. We can gradually
# enable these tests at a later stage if necessary.
if !USING_GCC_3_4_4_BACKEND_COMPILER
# TOO (2/17/11): Errors with Tensilica's Xtensa compilers. 
if !USING_XTENSA_BACKEND_COMPILER
#  SUBDIRS += astInterfaceTests astInliningTests ompLoweringTests astFileIOTests programAnalysisTests astOutliningTests loopProcessingTests fileLocation_tests
if !ROSE_USE_INSURE
   SUBDIRS += astInterfaceTests astInliningTests ompLoweringTests
   SUBDIRS += programAnalysisTests astOutliningTests loopProcessingTests fileLocation_tests
   SUBDIRS += astFileIOTests
endif

endif # !USING_XTENSA_BACKEND_COMPILER
endif # !USING_GCC_3_4_4_BACKEND_COMPILER

endif # ROSE_BUILD_CXX_LANGUAGE_SUPPORT

# endif # USING_GNU_COMPILER

if !ROSE_USE_INSURE
# DQ (4/23/2011): At least one of the files (process_cmdline.o) in this directory does not seem 
# to compile with Insure++ (it hangs for Hudson, but I can't reproduce the problem locally).
# This was not a problem the second time that Insure++ compiled ROSE.

# DQ (8/23/2013): This is a part of SUBDIRS above and is redundant here.
# SUBDIRS += roseHPCToolkitTests utilTests
# SUBDIRS += roseHPCToolkitTests

endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   SUBDIRS += PHPTests
endif

check-local:
	@echo "*******************************************"
	@echo "*******************************************"
	@echo "Passed tests for tests/roseTests directory."
	@echo "*******************************************"
	@echo "*******************************************"

