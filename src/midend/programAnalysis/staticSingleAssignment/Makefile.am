include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES =  libSSA.la

libSSA_la_DEPENDENCIES =
libSSA_la_SOURCES = staticSingleAssignmentCalculation.C staticSingleAssignmentQueries.C uniqueNameTraversal.C defsAndUsesTraversal.C \
		reachingDef.C staticSingleAssignmentInterprocedural.C
pkginclude_HEADERS = staticSingleAssignment.h uniqueNameTraversal.h defsAndUsesTraversal.h iteratedDominanceFrontier.h \
		reachingDef.h controlDependence.h dataflowCfgFilter.h boostGraphCFG.h
		
EXTRA_DIST = KNOWN_BUGS.txt
