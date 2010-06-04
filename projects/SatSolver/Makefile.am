include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lz

# DQ (5/28/2010): It is not enought to have this controled by an automake conditional.
# these need to be commented out explicitly to avoid requiring that 
# the sat solver source code be distributed.
# bin_PROGRAMS = sat
# EXTRA_DIST = 
# sat_SOURCES = 
# if ROSE_USE_SAT
#    sat_SOURCES += sat.C main.C
#    EXTRA_DIST += sat.h dimacsParser.h
# endif
# bin_PROGRAMS = sat
# sat_SOURCES = sat.C main.C
# EXTRA_DIST = sat.h dimacsParser.h

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/projects/SatSolver: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"
