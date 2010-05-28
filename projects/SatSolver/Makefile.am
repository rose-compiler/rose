include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lz

if ROSE_USE_SAT
  bin_PROGRAMS = sat
  sat_SOURCES = sat.C main.C

# EXTRA_DIST = sat.h dimacsParser.h
endif

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/projects/SatSolver: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"
