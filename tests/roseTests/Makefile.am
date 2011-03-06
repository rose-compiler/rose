include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS =  \
#      PHPTests \
#      astFileIOTests \
#      astInliningTests \
#      astInterfaceTests \
#      astMergeTests \
#      astOutliningTests \
#      astPerformanceTests \
#      astProcessingTests \
#      astQueryTests \
#      astRewriteTests \
#      astSymbolTableTests \
#      astTokenStreamTests \
#      binaryTests \
#      loopProcessingTests \
#      ompLoweringTests \
#      programAnalysisTests \
#      programTransformationTests \
#      roseHPCToolkitTests \
#      utilTests \
#      fileLocation_tests \
#      graph_tests \
#      mergeTraversal_tests

# DQ (4/17/2010): These directories are now organized based on what sets of languages 
# are enabled in ROSE (including binary analysis).
SUBDIRS =
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += astMergeTests astPerformanceTests \
              astProcessingTests astQueryTests astRewriteTests astSymbolTableTests astTokenStreamTests \
              programTransformationTests \
              roseHPCToolkitTests utilTests graph_tests mergeTraversal_tests \
	       	  astLValueTests
# TOO (2/16/2011): Errors with Thrifty (GCC 3.4.4) backend compiler. We can gradually
# enable these tests at a later stage if necessary.
if !USING_GCC_3_4_4_BACKEND_COMPILER
# TOO (2/17/11): Errors with Tensilica's Xtensa compilers. 
if !USING_XTENSA_BACKEND_COMPILER
   SUBDIRS += astInterfaceTests astInliningTests ompLoweringTests astFileIOTests programAnalysisTests astOutliningTests loopProcessingTests fileLocation_tests
endif 
endif
endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   SUBDIRS += PHPTests
endif

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   SUBDIRS += binaryTests
endif

if ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
   SUBDIRS += cudaTests
endif

if ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT
   SUBDIRS += openclTests
endif

check-local:
	@echo "**********************************************************************************"
	@echo "****** ROSE/tests/roseTests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************"
