include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# This file does not compile using ROSE (skip testing it for now)
if !ROSE_USING_ROSE_ANALYSIS
   bin_PROGRAMS = headerFilesGraphGenerator 
   headerFilesGraphGenerator_SOURCES = headerFilesGraphGenerator.C
endif

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
