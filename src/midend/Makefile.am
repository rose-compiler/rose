# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

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
mProgramTransformationPath=programTransformation#

#
# DQ (10/29/2010): We have to include "binaryAnalyses" directory so that its header files
# will be installed by "make install", even with language only options are used.
#
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS = \
	astDiagnostics \
	astProcessing \
	astQuery \
	binaryAnalyses
else
SUBDIRS = \
	abstractHandle \
	abstractMemoryObject \
	astDiagnostics \
	astProcessing \
	astQuery \
	astRewriteMechanism \
	astUtil \
	programAnalysis \
	programTransformation/loopProcessing \
	binaryAnalyses
endif

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

INCLUDES=\
	$(ROSE_INCLUDES) \
	-I${top_srcdir}/src/midend/programAnalysis/CFG

noinst_LTLIBRARIES=\
	libmidend.la

lib_LIBRARIES=\
	$(mProgramTransformation_libLibraries)

libmidend_la_SOURCES=\
	$(mProgramTransformation_la_sources)

include_HEADERS = \
	$(mProgramTransformation_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mProgramTransformation_extraDist)

clean-local:
	$(mProgramTransformation_cleanLocal)

