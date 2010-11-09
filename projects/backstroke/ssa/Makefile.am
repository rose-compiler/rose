include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke/slicing

# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_LIBRARIES =  libSSA.a

libSSA_a_DEPENDENCIES =
libSSA_a_SOURCES = staticSingleAssignmentCalculation.C staticSingleAssignmentQueries.C uniqueNameTraversal.C defsAndUsesTraversal.C
include_HEADERS = staticSingleAssignment.h uniqueNameTraversal.h defsAndUsesTraversal.h

# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/ssa: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
