# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

include $(srcdir)/partialRedundancyElimination/Makefile_variables
include $(srcdir)/finiteDifferencing/Makefile_variables

mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mptFiniteDifferencingPath=$(srcdir)/finiteDifferencing#

SUBDIRS = \
	functionCallNormalization \
	constantFolding \
	implicitCodeGeneration \
	astInlining \
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
	$(mptFiniteDifferencing_la_source)

include_HEADERS = \
	$(mptPartialRedundancyElimination_includeHeaders) \
	$(mptFiniteDifferencing_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mptPartialRedundancyElimination_extraDist) \
	$(mptFiniteDifferencing_extraDist)

clean-local:
	$(mptPartialRedundancyElimination_cleanLocal)
	$(mptFiniteDifferencing_cleanLocal)

