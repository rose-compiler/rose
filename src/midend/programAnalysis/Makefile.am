# Non-recursive Makefile
# dominatorTreesAndDominanceFrontiers is not utilized (config/support-rose.m4 does not
# generate a Makefile for it)

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AUTOMAKE_OPTIONS = subdir-objects

# DQ (10/7/2015): We don't want this to depend on this variable which is about what 
# backend compiler is used more than what compiler is used to compile ROSE itself.
#if ROSE_USING_GCC_VERSION_LATER_4_6
#SUBDIRS = staticSingleAssignment ssaUnfilteredCfg
#else
#SUBDIRS = staticSingleAssignment ssaUnfilteredCfg systemDependenceGraph
#endif

# DQ (4/3/2017): Boost 1.62 is not working due to internal Boost header files issues (Boost group suggestes using alternative version).
#SUBDIRS = staticSingleAssignment ssaUnfilteredCfg
SUBDIRS = 
if !ROSE_USING_BOOST_VERSION_1_62
# SUBDIRS += staticSingleAssignment
  SUBDIRS += staticSingleAssignment ssaUnfilteredCfg
else
# SUBDIRS += staticSingleAssignment ssaUnfilteredCfg
endif
# SUBDIRS += ssaUnfilteredCfg

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#>..e.g. include $(cxxCodeGenerationPath)/Makefile_variables
#
include $(srcdir)/arithmeticIntensity/Makefile_variables
include $(srcdir)/annotationLanguageParser/Makefile_variables
include $(srcdir)/CallGraphAnalysis/Makefile_variables
include $(srcdir)/CFG/Makefile_variables
include $(srcdir)/bitvectorDataflow/Makefile_variables
include $(srcdir)/VirtualFunctionAnalysis/Makefile_variables
include $(srcdir)/defUseAnalysis/Makefile_variables
include $(srcdir)/distributedMemoryAnalysis/Makefile_variables
include $(srcdir)/dominanceAnalysis/Makefile_variables
include $(srcdir)/genericDataflow/Makefile_variables
include $(srcdir)/OAWrap/Makefile_variables
include $(srcdir)/pointerAnal/Makefile_variables
include $(srcdir)/staticInterproceduralSlicing/Makefile_variables
include $(srcdir)/valuePropagation/Makefile_variables
include $(srcdir)/variableRenaming/Makefile_variables
include $(srcdir)/EditDistance/Makefile_variables


mpaArithmeticIntensityPath=$(srcdir)/arithmeticIntensity#
mpaAnnotationLanguageParserPath=$(srcdir)/annotationLanguageParser#
mpaCallGraphAnalysisPath=$(srcdir)/CallGraphAnalysis#
mpaCFGPath=$(srcdir)/CFG#
mpaBitvectorDataflowPath=$(srcdir)/bitvectorDataflow#
mpaVirtualFunctionAnalysisPath=$(srcdir)/VirtualFunctionAnalysis#
mpaDefUseAnalysisPath=$(srcdir)/defUseAnalysis#
mpaDistributedMemoryAnalysisPath=$(srcdir)/distributedMemoryAnalysis#
mpaDominanceAnalysisPath=$(srcdir)/dominanceAnalysis#
mpaGenericDataflowPath=$(srcdir)/genericDataflow#
mpaOAWrapPath=$(srcdir)/OAWrap#
mpaPointerAnalPath=$(srcdir)/pointerAnal#
mpaStaticInterproceduralSlicingPath=$(srcdir)/staticInterproceduralSlicing#
mpaValuePropagationPath=$(srcdir)/valuePropagation#
mpaVariableRenamingPath=$(srcdir)/variableRenaming#
mpaEditDistancePath=$(srcdir)/EditDistance#

###############################################################################

# Taken from midend/programAnalysis/annotationLanguageParser
AM_CXXFLAGS=\
	-D__LOCALSTL_H


AM_CPPFLAGS_OMP=\
	$(mpaDefUseAnalysis_includesOmp) \
	$(mpaVariableRenaming_includesOmp)


AM_CPPFLAGS=\
	$(ROSE_INCLUDES) \
	$(mpaArithmeticIntensity_includes) \
	$(mpaCallGraphAnalysis_includes) \
	$(mpaCFG_includes) \
	$(mpaBitvectorDataflow_includes) \
	$(mpaVirtualFunctionAnalysis_includes) \
	$(mpaDefUseAnalysis_includes) \
	$(mpaDistributedMemoryAnalysis_includes) \
	$(mpaGenericDataflow_includes) \
	$(mpaOAWrap_includes) \
	$(mpaPointerAnal_includes) \
	$(mpaSideEffectAnalysis_includes) \
	$(mpaStaticInterproceduralSlicing_includes) \
	$(mpaValuePropagation_includes) \
	$(mpaVariableRenaming_includes) \
	$(mpaEditDistance_includes) \
	$(AM_CPPFLAGS_OMP)


BUILT_SOURCES=\
	$(mpaAnnotationLanguageParser_builtSources)


noinst_LTLIBRARIES=\
	libprogramAnalysis.la \
	$(mpaAnnotationLanguageParser_noinstLTLIBRARIES) \
	$(mpaDistributedMemoryAnalysis_noinstLTLIBRARIES)

##################################################################

libprogramAnalysis_la_SOURCES=\
	$(mpaArithmeticIntensity_la_sources) \
	$(mpaCallGraphAnalysis_la_sources) \
	$(mpaCFG_la_sources) \
	$(mpaBitvectorDataflow_la_sources) \
	$(mpaVirtualFunctionAnalysis_la_sources) \
	$(mpaDefUseAnalysis_la_sources) \
	$(mpaGenericDataflow_la_sources) \
	$(mpaOAWrap_la_sources) \
	$(mpaPointerAnal_la_sources) \
	$(mpaSideEffectAnalysis_la_sources) \
	$(mpaStaticInterproceduralSlicing_la_sources) \
	$(mpaValuePropagation_la_sources) \
	$(mpaVariableRenaming_la_sources) \
	$(mpaEditDistance_la_sources)

# DQ (10/7/2015): We don't want this to depend on this variable which is about what 
# backend compiler is used more than what compiler is used to compile ROSE itself.
# if ROSE_USING_GCC_VERSION_LATER_4_6
# libprogramAnalysis_la_LIBADD = staticSingleAssignment/libSSA.la ssaUnfilteredCfg/libSSA_UnfilteredCfg.la
# else
# libprogramAnalysis_la_LIBADD = staticSingleAssignment/libSSA.la ssaUnfilteredCfg/libSSA_UnfilteredCfg.la systemDependenceGraph/libSDG.la
# endif

# DQ (4/3/2017): Boost 1.62 is a problem and it is suggested that people not use it.  We disable some minimal parts of ROSE to address this.
# libprogramAnalysis_la_LIBADD = staticSingleAssignment/libSSA.la ssaUnfilteredCfg/libSSA_UnfilteredCfg.la
if !ROSE_USING_BOOST_VERSION_1_62
   libprogramAnalysis_la_LIBADD = staticSingleAssignment/libSSA.la ssaUnfilteredCfg/libSSA_UnfilteredCfg.la
else
   libprogramAnalysis_la_LIBADD = 
endif

pkginclude_HEADERS=\
	$(mpaArithmeticIntensity_includeHeaders) \
	$(mpaCallGraphAnalysis_includeHeaders) \
	$(mpaCFG_includeHeaders) \
	$(mpaBitvectorDataflow_includeHeaders) \
	$(mpaVirtualFunctionAnalysis_includeHeaders) \
	$(mpaDefUseAnalysis_includeHeaders) \
	$(mpaDistributedMemoryAnalysis_includeHeaders) \
	$(mpaDominanceAnalysis_includeHeaders) \
	$(mpaGenericDataflow_includeHeaders) \
	$(mpaOAWrap_includeHeaders) \
	$(mpaPointerAnal_includeHeaders) \
	$(mpaSideEffectAnalysis_includeHeaders) \
	$(mpaStaticInterproceduralSlicing_includeHeaders) \
	$(mpaValuePropagation_includeHeaders) \
	$(mpaVariableRenaming_includeHeaders) \
	$(mpaEditDistance_includeHeaders)


noinst_HEADERS=\
	$(mpaAnnotationLanguageParser_noinstHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	OpenAnalysis \
	PolicyControl.txt \
	$(mpaArithmeticIntensity_extraDist) \
	$(mpaAnnotationLanguageParser_extraDist) \
	$(mpaCallGraphAnalysis_extraDist) \
	$(mpaCFG_extraDist) \
	$(mpaBitvectorDataflow_extraDist) \
	$(mpaVirtualFunctionAnalysis_extraDist) \
	$(mpaDefUseAnalysis_extraDist) \
	$(mpaDistributedMemoryAnalysis_extraDist) \
	$(mpaDominanceAnalysis_extraDist) \
	$(mpaGenericDataflow_extraDist) \
	$(mpaOAWrap_extraDist) \
	$(mpaPointerAnal_extraDist) \
	$(mpaSideEffectAnalysis_extraDist) \
	$(mpaStaticInterproceduralSlicing_extraDist) \
	$(mpaValuepropagation_extraDist) \
	$(mpaVariableRenaming_extraDist) \
	$(mpaEditDistance_extraDist)


clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
	$(mpaArithmeticIntensity_cleanLocal) \
	$(mpaAnnotationLanguageParser_cleanLocal)
	$(mpaCallGraphAnalysis_cleanLocal)
	$(mpaCFG_cleanLocal)
	$(mpaBitvectorDataflow_cleanLocal)
	$(mpaVirtualFunctionAnalysis_cleanLocal)
	$(mpaDefUseAnalysis_cleanLocal)
	$(mpaDominanceAnalysis_cleanLocal)
	$(mpaGenericDataflow_cleanLocal)
	$(mpaOAWrap_cleanLocal)
	$(mpaPointerAnal_cleanLocal)
	$(mpaStaticInterproceduralSlicing_cleanLocal)
	$(mpaValuePropagation_cleanLocal)
	$(mpaVariableRenaming_cleanLocal)
	$(mpaEditDistance_cleanLocal)


distclean-local:
#	rm -rf Templates.DB
	$(mpaArithmeticIntensity_distCleanLocal) \
	$(mpaCallGraphAnalysis_distCleanLocal)
	$(mpaCFG_distCleanLocal)
	$(mpaBitvectorDataflow_distCleanLocal)
	$(mpaVirtualFunctionAnalysis_distCleanLocal)
	$(mpaDefUseAnalysis_distCleanLocal)
	$(mpaDominanceAnalysis_distCleanLocal)
	$(mpaGenericDataflow_distCleanLocal)
	$(mpaOAWrap_distCleanLocal)
	$(mpaPointerAnal_distCleanLocal)
	$(mpaStaticInterproceduralSlicing_distCleanLocal)
	$(mpaValuePropagation_distCleanLocal)
	$(mpaVariableRenaming_distCleanLocal)
	$(mpaEditDistance_distCleanLocal)




## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
