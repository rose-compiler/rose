# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/astDiagnostics/Makefile_variables
include $(srcdir)/astProcessing/Makefile_variables
include $(srcdir)/astQuery/Makefile_variables
include $(srcdir)/programTransformation/Makefile_variables
include $(srcdir)/programTransformation/partialRedundancyElimination/Makefile_variables
include $(srcdir)/programTransformation/finiteDifferencing/Makefile_variables
include $(srcdir)/programTransformation/functionCallNormalization/Makefile_variables
include $(srcdir)/programTransformation/constantFolding/Makefile_variables
include $(srcdir)/programTransformation/implicitCodeGeneration/Makefile_variables
include $(srcdir)/programTransformation/astInlining/Makefile_variables
include $(srcdir)/programTransformation/astOutlining/Makefile_variables
include $(srcdir)/programTransformation/ompLowering/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mAstDiagnosticsPath=astDiagnostics#
mAstProcessingPath=astProcessing#
mAstQueryPath=astQuery#
mProgramTransformationPath=programTransformation#

#
# DQ (10/29/2010): We have to include "binaryAnalyses" directory so that its header files
# will be installed by "make install", even with language only options are used.
#
SUBDIRS = \
	binaryAnalyses
if !ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS += \
	abstractHandle \
	abstractMemoryObject \
	astRewriteMechanism \
	astUtil \
	programAnalysis \
	programTransformation/loopProcessing
endif

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

INCLUDES=\
	$(ROSE_INCLUDES) \
	-I${top_srcdir}/src/midend/programAnalysis/CFG

noinst_LTLIBRARIES=\
	libmidend.la

lib_LIBRARIES=\
	$(mProgramTransformation_libLibraries)

libmidend_la_SOURCES=\
	$(mAstDiagnostics_la_sources) \
	$(mAstProcessing_la_sources) \
	$(mAstQuery_la_sources) \
	$(mProgramTransformation_la_sources)

# astProcessing _la_LIBADD = $(CXX_TEMPLATE_OBJECTS)
libmidend_la_LIBADD=\
	$(mAstProcessing_libadd)

libmidend_la_DEPENDENCIES=\
	$(mAstQuery_la_dependencies)

include_HEADERS = \
	$(mAstDiagnostics_includeHeaders) \
	$(mAstProcessing_includeHeaders) \
	$(mAstQuery_includeHeaders) \
	$(mProgramTransformation_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mAstDiagnostics_extraDist) \
	$(mAstProcessing_extraDist) \
	$(mAstQuery_extraDist) \
	$(mProgramTransformation_extraDist)

clean-local:
	$(mAstDiagnostics_cleanLocal)
	$(mAstProcessing_cleanLocal)
	$(mAstQuery_cleanLocal)
	$(mProgramTransformation_cleanLocal)

distclean-local:
	$(mAstProcessing_distCleanLocal)
	$(mAstQuery_distCleanLocal)

