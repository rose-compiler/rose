# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

include $(srcdir)/partialRedundancyElimination/Makefile_variables
include $(srcdir)/finiteDifferencing/Makefile_variables
include $(srcdir)/functionCallNormalization/Makefile_variables
include $(srcdir)/constantFolding/Makefile_variables
include $(srcdir)/implicitCodeGeneration/Makefile_variables
include $(srcdir)/astInlining/Makefile_variables

mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mptFiniteDifferencingPath=$(srcdir)/finiteDifferencing#
mptFunctionCallNormalizationPath=$(srcdir)/functionCallNormalization#
mptConstantFoldingPath=$(srcdir)/constantFolding#
mptImplicitCodeGenerationPath=$(srcdir)/implicitCodeGeneration#
mptAstInliningPath=$(srcdir)/astInlining#

SUBDIRS = \
	astOutlining \
	loopProcessing \
	ompLowering

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

INCLUDES=\
	$(ROSE_INCLUDES) \
	$(mptPartialRedundancyElimination_includes) \
	$(mptFiniteDifferencing_includes)

noinst_LTLIBRARIES=\
	libprogramTransformation.la

libprogramTransformation_la_SOURCES = \
	$(mptPartialRedundancyElimination_la_sources) \
	$(mptFiniteDifferencing_la_source) \
	$(mptFunctionCallNormalization_la_sources) \
	$(mptConstantFolding_la_sources) \
	$(mptImplicitCodeGeneration_la_sources) \
	$(mptAstInlining_la_sources)

include_HEADERS = \
	$(mptPartialRedundancyElimination_includeHeaders) \
	$(mptFiniteDifferencing_includeHeaders) \
	$(mptFunctionCallNormalization_includeHeaders) \
	$(mptConstantFolding_includeHeaders) \
	$(mptImplicitCodeGeneration_includeHeaders) \
	$(mptAstInlining_includeHeaders)

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
	$(mptAstInlining_extraDist)

clean-local:
	$(mptPartialRedundancyElimination_cleanLocal)
	$(mptFiniteDifferencing_cleanLocal)
	$(mptFunctionCallNormalization_cleanLocal)
	$(mptConstantFolding_cleanLocal)
	$(mptAstInlining_cleanLocal)

