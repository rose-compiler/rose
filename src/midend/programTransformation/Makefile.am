# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

include $(srcdir)/partialRedundancyElimination/Makefile_variables
mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#

SUBDIRS = \
	finiteDifferencing \
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
	$(mptPartialRedundancyElimination_includes)

noinst_LTLIBRARIES=\
	libprogramTransformation.la

libprogramTransformation_la_SOURCES = \
	$(mptPartialRedundancyElimination_la_sources)

include_HEADERS = \
	$(mptPartialRedundancyElimination_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mptPartialRedundancyElimination_extraDist)

clean-local:
	$(mptPartialRedundancyElimination_cleanLocal)

