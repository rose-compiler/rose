include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES =  libSSA.la

libSSA_la_DEPENDENCIES =
libSSA_la_SOURCES = staticSingleAssignmentCalculation.C staticSingleAssignmentQueries.C uniqueNameTraversal.C defsAndUsesTraversal.C \
		reachingDef.C staticSingleAssignmentInterprocedural.C
include_HEADERS = staticSingleAssignment.h uniqueNameTraversal.h defsAndUsesTraversal.h iteratedDominanceFrontier.h \
		reachingDef.h controlDependence.h dataflowCfgFilter.h backstrokeCFG.h
		
EXTRA_DIST = KNOWN_BUGS.txt