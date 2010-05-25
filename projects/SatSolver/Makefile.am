include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

if ROSE_USE_SAT
  bin_PROGRAMS = sat
  sat_SOURCES = sat.C
endif

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = sat.h dimacsParser.h

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/projects/SatSolver: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"
