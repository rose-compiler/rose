# Non-recursive Makefile
# dominatorTreesAndDominanceFrontiers is not utilized (config/support-rose.m4 does not
# generate a Makefile for it)

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#>..e.g. include $(cxxCodeGenerationPath)/Makefile_variables
#
include $(srcdir)/annotationLanguageParser/Makefile_variables
include $(srcdir)/CallGraphAnalysis/Makefile_variables
include $(srcdir)/CFG/Makefile_variables
include $(srcdir)/dataflowAnalysis/Makefile_variables
include $(srcdir)/defUseAnalysis/Makefile_variables
include $(srcdir)/distributedMemoryAnalysis/Makefile_variables
include $(srcdir)/dominanceAnalysis/Makefile_variables
include $(srcdir)/graphAnalysis/Makefile_variables
include $(srcdir)/OAWrap/Makefile_variables
include $(srcdir)/pointerAnal/Makefile_variables
include $(srcdir)/sideEffectAnalysis/Makefile_variables
include $(srcdir)/staticInterproceduralSlicing/Makefile_variables
include $(srcdir)/valuePropagation/Makefile_variables
include $(srcdir)/variableRenaming/Makefile_variables


mpaAnnotationLanguageParserPath=$(srcdir)/annotationLanguageParser#
mpaCallGraphAnalysisPath=$(srcdir)/CallGraphAnalysis#
mpaCFGPath=$(srcdir)/CFG#
mpaDataflowAnalysisPath=$(srcdir)/dataflowAnalysis#
mpaDefUseAnalysisPath=$(srcdir)/defUseAnalysis#
mpaDistributedMemoryAnalysisPath=$(srcdir)/distributedMemoryAnalysis#
mpaDominanceAnalysisPath=$(srcdir)/dominanceAnalysis#
mpaGraphAnalysisPath=$(srcdir)/graphAnalysis#
mpaOAWrapPath=$(srcdir)/OAWrap#
mpaPointerAnalPath=$(srcdir)/pointerAnal#
mpaSideEffectAnalysisPath=$(srcdir)/sideEffectAnalysis#
mpaStaticInterproceduralSlicingPath=$(srcdir)/staticInterproceduralSlicing#
mpaValuePropagationPath=$(srcdir)/valuePropagation#
mpaVariableRenamingPath=$(srcdir)/variableRenaming#

###############################################################################

# Taken from midend/programAnalysis/annotationLanguageParser
AM_CXXFLAGS=\
	-D__LOCALSTL_H


INCLUDES_OMP=\
	$(mpaDefUseAnalysis_includesOmp) \
	$(mpaVariableRenaming_includesOmp)


INCLUDES=\
	$(ROSE_INCLUDES) \
	$(mpaCallGraphAnalysis_includes) \
	$(mpaCFG_includes) \
	$(mpaDataflowAnalysis_includes) \
	$(mpaDefUseAnalysis_includes) \
	$(mpaDistributedMemoryAnalysis_includes) \
	$(mpaOAWrap_includes) \
	$(mpaPointerAnal_includes) \
	$(mpaSideEffectAnalysis_includes) \
	$(mpaStaticInterproceduralSlicing_includes) \
	$(mpaValuePropagation_includes) \
	$(mpaVariableRenaming_includes) \
	$(INCLUDES_OMP)


BUILT_SOURCES=\
	$(mpaAnnotationLanguageParser_builtSources)


noinst_LTLIBRARIES=\
	libprogramAnalysis.la \
	$(mpaAnnotationLanguageParser_noinstLTLIBRARIES) \
	$(mpaDistributedMemoryAnalysis_noinstLTLIBRARIES)

##################################################################

libprogramAnalysis_la_SOURCES=\
	$(mpaCallGraphAnalysis_la_sources) \
	$(mpaCFG_la_sources) \
	$(mpaDataflowAnalysis_la_sources) \
	$(mpaDefUseAnalysis_la_sources) \
	$(mpaGraphAnalysis_la_sources) \
	$(mpaOAWrap_la_sources) \
	$(mpaPointerAnal_la_sources) \
	$(mpaSideEffectAnalysis_la_sources) \
	$(mpaStaticInterproceduralSlicing_la_sources) \
	$(mpaValuePropagation_la_sources) \
	$(mpaVariableRenaming_la_sources)


include_HEADERS=\
	$(mpaCallGraphAnalysis_includeHeaders) \
	$(mpaCFG_includeHeaders) \
	$(mpaDataflowAnalysis_includeHeaders) \
	$(mpaDefUseAnalysis_includeHeaders) \
	$(mpaDistributedMemoryAnalysis_includeHeaders) \
	$(mpaDominanceAnalysis_includeHeaders) \
	$(mpaGraphAnalysis_includeHeaders) \
	$(mpaOAWrap_includeHeaders) \
	$(mpaPointerAnal_includeHeaders) \
	$(mpaSideEffectAnalysis_includeHeaders) \
	$(mpaStaticInterproceduralSlicing_includeHeaders) \
	$(mpaValuePropagation_includeHeaders) \
	$(mpaVariableRenaming_includeHeaders)


noinst_HEADERS=\
	$(mpaAnnotationLanguageParser_noinstHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	OpenAnalysis \
	$(mpaAnnotationLanguageParser_extraDist) \
	$(mpaCallGraphAnalysis_extraDist) \
	$(mpaCFG_extraDist) \
	$(mpaDataflowAnalysis_extraDist) \
	$(mpaDefUseAnalysis_extraDist) \
	$(mpaDistributedMemoryAnalysis_extraDist) \
	$(mpaDominanceAnalysis_extraDist) \
	$(mpaGraphAnalysis_extraDist) \
	$(mpaOAWrap_extraDist) \
	$(mpaPointerAnal_extraDist) \
	$(mpaSideEffectAnalysis_extraDist) \
	$(mpaStaticInterproceduralSlicing_extraDist) \
	$(mpaValuepropagation_extraDist) \
	$(mpaVariableRenaming_extraDist)


clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	$(mpaAnnotationLanguageParser_cleanLocal)
	$(mpaCallGraphAnalysis_cleanLocal)
	$(mpaCFG_cleanLocal)
	$(mpaDataflowAnalysis_cleanLocal)
	$(mpaDefUseAnalysis_cleanLocal)
	$(mpaDominanceAnalysis_cleanLocal)
	$(mpaGraphAnalysis_cleanLocal)
	$(mpaOAWrap_cleanLocal)
	$(mpaPointerAnal_cleanLocal)
	$(mpaStaticInterproceduralSlicing_cleanLocal)
	$(mpaValuePropagation_cleanLocal)
	$(mpaVariableRenaming_cleanLocal)


distclean-local:
	rm -rf Templates.DB
	$(mpaCallGraphAnalysis_distCleanLocal)
	$(mpaCFG_distCleanLocal)
	$(mpaDataflowAnalysis_distCleanLocal)
	$(mpaDefUseAnalysis_distCleanLocal)
	$(mpaDominanceAnalysis_distCleanLocal)
	$(mpaGraphAnalysis_distCleanLocal)
	$(mpaOAWrap_distCleanLocal)
	$(mpaPointerAnal_distCleanLocal)
	$(mpaStaticInterproceduralSlicing_distCleanLocal)
	$(mpaValuePropagation_distCleanLocal)
	$(mpaVariableRenaming_distCleanLocal)




## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
