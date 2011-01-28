include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#    e.g. include cxxCodeGenerationPath/Makefile_variables
#
include $(srcdir)/astInlining/Makefile_variables
include $(srcdir)/astOutlining/Makefile_variables
include $(srcdir)/constantFolding/Makefile_variables
include $(srcdir)/finiteDifferencing/Makefile_variables
include $(srcdir)/functionCallNormalization/Makefile_variables
include $(srcdir)/implicitCodeGeneration/Makefile_variables
include $(srcdir)/loopProcessing/Makefile_variables
include $(srcdir)/loopProcessing/computation/Makefile_variables
include $(srcdir)/loopProcessing/depGraph/Makefile_variables
include $(srcdir)/loopProcessing/depInfo/Makefile_variables
include $(srcdir)/loopProcessing/driver/Makefile_variables
include $(srcdir)/loopProcessing/outsideInterface/Makefile_variables
include $(srcdir)/loopProcessing/prepostTransformation/Makefile_variables
include $(srcdir)/loopProcessing/slicing/Makefile_variables
include $(srcdir)/ompLowering/Makefile_variables
include $(srcdir)/partialRedundancyElimination/Makefile_variables


mptAstInliningPath=$(srcdir)/astInlining#
mptAstOutliningPath=$(srcdir)/astOutlining#
mptConstantFoldingPath=$(srcdir)/constantFolding#
mptFiniteDifferencingPath=$(srcdir)/finiteDifferencing#
mptFunctionCallNormalizationPath=$(srcdir)/functionCallNormalization#
mptImplicitCodeGenerationPath=$(srcdir)/implicitCodeGeneration#
## loop processing
mptLoopProcessingPath=$(srcdir)/loopProcessing#
mpt_lpComputationPath=$(mptLoopProcessingPath)/computation#
mpt_lpDepGraphPath=$(mptLoopProcessingPath)/depGraph#
mpt_lpDepInfoPath=$(mptLoopProcessingPath)/depInfo#
mpt_lpDriverPath=$(mptLoopProcessingPath)/driver#
mpt_lpOutsideInterfacePath=$(mptLoopProcessingPath)/outsideInterface#
mpt_lpPrepostTransformationPath=$(mptLoopProcessingPath)/prepostTransformation#
mpt_lpSlicingPath=$(mptLoopProcessingPath)/slicing#
##
mptOmpLoweringPath=$(srcdir)/ompLowering#
mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#

###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES) \
	$(mptFiniteDifferencing_includes) \
	$(mptLoopProcessing_includes) \
	$(mptPartialRedundancyElimination_includes)


noinst_LTLIBRARIES = libprogramTransformation.la


lib_LIBRARIES=\
	$(mptOmpLowering_libLibraries)

##################################################################

libprogramTransformation_la_SOURCES=\
	$(mptAstInlining_la_sources) \
	$(mptAstOutlining_la_sources) \
	$(mptConstantFolding_la_sources) \
	$(mptFiniteDifferencing_la_sources) \
	$(mptFunctionCallNormalization_la_sources) \
	$(mptImplicitCodeGeneration_la_sources) \
	$(mptLoopProcessing_la_sources) \
	$(mptOmpLowering_la_sources) \
	$(mptPartialRedundancyElimination_la_sources)


include_HEADERS=\
	$(mptAstInlining_includeHeaders) \
	$(mptAstOutlining_includeHeaders) \
	$(mptConstantFolding_includeHeaders) \
	$(mptFiniteDifferencing_includeHeaders) \
	$(mptFunctionCallNormalization_includeHeaders) \
	$(mptImplicitCodeGeneration_includeHeaders) \
	$(mptLoopProcessing_includeHeaders) \
	$(mptOmpLowering_includeHeaders) \
	$(mptPartialRedundancyElimination_includeHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	$(mptAstInlining_extraDist) \
	$(mptAstOutlining_extraDist) \
	$(mptConstantFolding_extraDist) \
	$(mptFiniteDifferencing_extraDist) \
	$(mptFunctionCallNormalization_extraDist) \
	$(mptImplicitCodeGeneration_extraDist) \
	$(mptLoopProcessing_extraDist) \
	$(mptOmpLowering_extraDist) \
	$(mptPartialRedundancyElimination_extraDist)


clean-local:
	$(mptAstInlining_cleanLocal)
	$(mptAstOutlining_cleanLocal)
	$(mptConstantFolding_cleanLocal)
	$(mptFiniteDifferencing_cleanLocal)
	$(mptFunctionCallNormalization_cleanLocal)
	$(mptLoopProcessing_cleanLocal)
	$(mptOmpLowering_cleanLocal)
	$(mptPartialRedundancyElimination_cleanLocal)


distclean-local:
	$(mptLoopProcessing_distCleanLocal)
