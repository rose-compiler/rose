
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(CallGraphAnalysis)
add_subdirectory(OAWrap)

# DQ (11/25/2009) Comment out under Windows.
if (NOT WIN32)
message ("Commented out use of OpenAnalysis in Windows.")
add_subdirectory(CFG)
endif (NOT WIN32)

add_subdirectory(dataflowAnalysis)
add_subdirectory(valuePropagation)
add_subdirectory(defUseAnalysis)
add_subdirectory(dominanceAnalysis)
add_subdirectory(staticInterproceduralSlicing)
add_subdirectory(pointerAnal)
add_subdirectory(annotationLanguageParser)
add_subdirectory(sideEffectAnalysis)
add_subdirectory(distributedMemoryAnalysis)
add_subdirectory(graphAnalysis)
add_subdirectory(variableRenaming)



#add_library( midend_pa STATIC ${midend_pa_src} )
#add_dependencies( midend_pa ROSETTA )

########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## DQ (5/3/2007): Old version before Christian's work was added.
##SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis
##          valuePropagation  dominatorTreesAndDominanceFrontiers
##          staticProgramSlicing proceduralSlicing
#
#SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis \
#          valuePropagation  defUseAnalysis dominanceAnalysis \
#          staticInterproceduralSlicing pointerAnal \
#	  annotationLanguageParser sideEffectAnalysis distributedMemoryAnalysis \
#          graphAnalysis
#
## DQ (9/6/2005): Commented out these directories since they
## represent code that used the older graph interface and they
## no longer compile!  These will be updated later.
## staticProgramSlicing proceduralSlicing
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
##dist-hook:
##	ln -s ../STL-link ${distdir}
#
## AUTOMAKE_OPTIONS = foreign
#
#EXTRA_DIST = OpenAnalysis
#
#
#
#
#
