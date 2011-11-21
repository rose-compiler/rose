# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/partialRedundancyElimination/Makefile_variables
include $(srcdir)/finiteDifferencing/Makefile_variables
include $(srcdir)/functionCallNormalization/Makefile_variables
include $(srcdir)/constantFolding/Makefile_variables
include $(srcdir)/implicitCodeGeneration/Makefile_variables
include $(srcdir)/astInlining/Makefile_variables
include $(srcdir)/astOutlining/Makefile_variables
include $(srcdir)/ompLowering/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mptPartialRedundancyEliminationPath=partialRedundancyElimination#
mptFiniteDifferencingPath=finiteDifferencing#
mptFunctionCallNormalizationPath=functionCallNormalization#
mptConstantFoldingPath=constantFolding#
mptImplicitCodeGenerationPath=implicitCodeGeneration#
mptAstInliningPath=astInlining#
mptOutliningPath=astOutlining#
mptOmpLoweringPath=ompLowering#

SUBDIRS = \
	loopProcessing

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

INCLUDES=\
	$(ROSE_INCLUDES) \
	-I${top_srcdir}/src/midend/programAnalysis/CFG

noinst_LTLIBRARIES=\
	libprogramTransformation.la

lib_LIBRARIES=\
	$(mptOmpLowering_lib_libraries)

libprogramTransformation_la_SOURCES = \
	$(mptPartialRedundancyElimination_la_sources) \
	$(mptFiniteDifferencing_la_source) \
	$(mptFunctionCallNormalization_la_sources) \
	$(mptConstantFolding_la_sources) \
	$(mptImplicitCodeGeneration_la_sources) \
	$(mptAstInlining_la_sources) \
	$(mptAstOutlining_la_sources) \
	$(mptOmpLowering_la_sources)

include_HEADERS = \
	$(mptPartialRedundancyElimination_includeHeaders) \
	$(mptFiniteDifferencing_includeHeaders) \
	$(mptFunctionCallNormalization_includeHeaders) \
	$(mptConstantFolding_includeHeaders) \
	$(mptImplicitCodeGeneration_includeHeaders) \
	$(mptAstInlining_includeHeaders) \
	$(mptAstOutlining_includeHeaders) \
	$(mptOmpLowering_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mptPartialRedundancyElimination_extraDist) \
	$(mptFiniteDifferencing_extraDist) \
	$(mptFunctionCallNormalization_extraDist) \
	$(mptConstantFolding_extraDist) \
	$(mptImplicitCodeGeneration_extraDist) \
	$(mptAstInlining_extraDist) \
	$(mptAstOutlining_extraDist) \
	$(mptOmpLowering_extraDist)

clean-local:
	$(mptPartialRedundancyElimination_cleanLocal)
	$(mptFiniteDifferencing_cleanLocal)
	$(mptFunctionCallNormalization_cleanLocal)
	$(mptConstantFolding_cleanLocal)
	$(mptAstInlining_cleanLocal)
	$(mptAstOutlining_cleanLocal)
	$(mptOmpLowering_cleanLocal)

