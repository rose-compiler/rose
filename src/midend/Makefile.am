# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Subdirectories
# ------------------------------------------------------------------------------

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/abstractHandle/Makefile_variables
include $(srcdir)/abstractLayer/Makefile_variables
include $(srcdir)/abstractMemoryObject/Makefile_variables
include $(srcdir)/astDiagnostics/Makefile_variables
include $(srcdir)/astProcessing/Makefile_variables
include $(srcdir)/astMatching/Makefile_variables
include $(srcdir)/astRewriteMechanism/Makefile_variables
include $(srcdir)/astQuery/Makefile_variables
include $(srcdir)/astSnippet/Makefile_variables
include $(srcdir)/astUtil/Makefile_variables
include $(srcdir)/astUtil/annotation/Makefile_variables
include $(srcdir)/astUtil/astInterface/Makefile_variables
include $(srcdir)/astUtil/astSupport/Makefile_variables
include $(srcdir)/astUtil/symbolicVal/Makefile_variables
include $(srcdir)/programTransformation/Makefile_variables
include $(srcdir)/programTransformation/partialRedundancyElimination/Makefile_variables
include $(srcdir)/programTransformation/finiteDifferencing/Makefile_variables
include $(srcdir)/programTransformation/functionCallNormalization/Makefile_variables
include $(srcdir)/programTransformation/constantFolding/Makefile_variables
include $(srcdir)/programTransformation/implicitCodeGeneration/Makefile_variables
include $(srcdir)/programTransformation/astInlining/Makefile_variables
include $(srcdir)/programTransformation/astOutlining/Makefile_variables
include $(srcdir)/programTransformation/ompLowering/Makefile_variables
include $(srcdir)/programTransformation/transformationTracking/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
mAbstractHandlePath=abstractHandle#
mAbstractLayerPath=abstractLayer#
mAbstractMemoryObjectPath=abstractMemoryObject#
mAstDiagnosticsPath=astDiagnostics#
mAstProcessingPath=astProcessing#
mAstMatchingPath=astMatching#
mAstRewriteMechanismPath=astRewriteMechanism#
mAstQueryPath=astQuery#
mAstSnippetPath=astSnippet#
mAstUtilPath=astUtil#
mProgramTransformationPath=programTransformation#

SUBDIRS = \
	BinaryAnalysis \
	programTransformation/extractFunctionArgumentsNormalization \
	programTransformation/singleStatementToBlockNormalization 

# DQ (12/2/2016): These directories have poorly formated Makefile.am files have
# and are a source of portability problem on Ubuntu systems (because they overload 
# the CXXFLAGS makefile variable).
#       MFB MDCG KLT

if !ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS += \
	programAnalysis \
	programTransformation/loopProcessing
endif

# ------------------------------------------------------------------------------
#  Library: programTransformation.la
# ------------------------------------------------------------------------------

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

AM_CPPFLAGS=\
	$(ROSE_INCLUDES) \
	-I${top_srcdir}/src/midend/programAnalysis/CFG \
	-I$(mAstUtilPath) \
	-I$(mAstUtilPath)/astInterface \
	-I$(mAstUtilPath)/symbolicVal \
	-I$(mAstUtilPath)/astSupport \
	-I$(top_srcdir)/src/util \
	-I$(top_srcdir)/src/util/support

noinst_LTLIBRARIES=\
	libmidend.la

lib_LTLIBRARIES=\
	$(mProgramTransformation_lib_ltlibraries)

#BUILT_SOURCES = $(mAstMatching_built_sources)

libmidend_la_SOURCES=\
	$(mAbstractHandle_la_sources) \
	$(mAbstractLayer_la_sources) \
	$(mAbstractMemoryObject_la_sources) \
	$(mAstDiagnostics_la_sources) \
	$(mAstProcessing_la_sources) \
	$(mAstMatching_la_sources) \
	$(mAstQuery_la_sources) \
	$(mAstSnippet_la_sources)
if !ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libmidend_la_SOURCES+=\
	$(mAstRewriteMechanism_la_sources) \
	$(mAstUtil_la_sources) \
	$(mProgramTransformation_la_sources)
endif

libmidend_la_DEPENDENCIES=\
	$(mAstQuery_la_dependencies)

pkginclude_HEADERS = \
	$(mAbstractHandle_includeHeaders) \
	$(mAbstractLayer_includeHeaders) \
	$(mAbstractMemoryObject_includeHeaders) \
	$(mAstDiagnostics_includeHeaders) \
	$(mAstProcessing_includeHeaders) \
	$(mAstMatching_includeHeaders) \
	$(mAstRewriteMechanism_includeHeaders) \
	$(mAstQuery_includeHeaders) \
	$(mAstSnippet_includeHeaders) \
	$(mAstUtil_includeHeaders) \
	$(mProgramTransformation_includeHeaders)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

EXTRA_DIST = \
	CMakeLists.txt \
	$(mAbstractHandle_extraDist) \
	$(mAbstractLayer_extraDist) \
	$(mAbstractMemoryObject_extraDist) \
	$(mAstDiagnostics_extraDist) \
	$(mAstProcessing_extraDist) \
	$(mAstMatching_extraDist) \
	$(mAstRewriteMechanism_extraDist) \
	$(mAstQuery_extraDist) \
	$(mAstUtil_extraDist) \
	$(mProgramTransformation_extraDist)

clean-local:
	$(mAbstractHandle_cleanLocal)
	$(mAbstractLayer_cleanLocal)
	$(mAbstractMemoryObject_cleanLocal)
	$(mAstDiagnostics_cleanLocal)
	$(mAstProcessing_cleanLocal)
	$(mAstMatching_cleanLocal)
	$(mAstRewriteMechanism_cleanLocal)
	$(mAstQuery_cleanLocal)
	$(mAstUtil_cleanLocal)
	$(mProgramTransformation_cleanLocal)

distclean-local:
	$(mAstProcessing_distCleanLocal)
	$(mAstMatching_distCleanLocal)
	$(mAstQuery_distCleanLocal)
	$(mAstUtil_distCleanLocal)

# Explicit rules because of multiple bison parsers in the
# library. These rules generate a unique name for the parser to avoid
# name conflicts with the default naming scheme. The generated parser
# is a tracked file. It's part of development to regenerate it if the
# parser is changed. This rule is in this Makefile.am because we only use
# variables from the subdirectory make files here.
regenerateparser:
#$(srcdir)/$(mAstMatchingPath)matcherparser.C: $(srcdir)/$(mAstMatchingPath)/matcherlexer.ll $(srcdir)/$(mAstMatchingPath)/matcherparser.yy
	$(AM_V_GEN)
	@$(YACC) $(YFLAGS) -d -p matcherparser $(srcdir)/$(mAstMatchingPath)/matcherparser.yy # generates y.tab.h/c
	@$(LEX) $(LFLAGS) $(AM_LFLAGS) -Pmatcherparser $(srcdir)/$(mAstMatchingPath)/matcherlexer.ll
	@mv y.tab.h $(srcdir)/$(mAstMatchingPath)/matcherparser.h
	@cat lex.yy.c y.tab.c > $(srcdir)/$(mAstMatchingPath)/matcherparser.C
	@rm lex.yy.c y.tab.c
$(srcdir)/$(mAstMatchingPath)/matcherparser.h: $(srcdir)/$(mAstMatchingPath)/matcherparser.C
