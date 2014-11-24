include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
include_HEADERS=
noinst_PROGRAMS=

###############################################################################################################################
#  PROGRAMS
###############################################################################################################################

noinst_PROGRAMS += analyzer
analyzer_SOURCES = analyzer.C
analyzer_CPPFLAGS = $(ROSE_INCLUDES)
analyzer_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += generateRoseCsvFile
generateRoseCsvFile_SOURCES = generateRoseCsvFile.C
generateRoseCsvFile_CPPFLAGS = $(ROSE_INCLUDES)
generateRoseCsvFile_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS += feasibility
feasibility_SOURCES = feasibility.C
feasibility_CPPFLAGS = $(ROSE_INCLUDES)
feasibility_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
