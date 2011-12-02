include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
include_HEADERS=
noinst_PROGRAMS=

###############################################################################################################################
#  PROGRAMS
###############################################################################################################################

# Compares ROSE function detection with IDA PRO function detection
noinst_PROGRAMS += idaCompare
idaCompare_SOURCES = idaCompare.C
idaCompare_CPPFLAGS = $(ROSE_INCLUDES)
idaCompare_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Modifies C/C++ source code so every function begins with something notable.
noinst_PROGRAMS += markFunctions
markFunctions_SOURCES = markFunctions.C
markFunctions_CPPFLAGS = $(ROSE_INCLUDES)
markFunctions_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# PStatistics demo
noinst_PROGRAMS += feasibility
feasibility_SOURCES = feasibility.C
feasibility_CPPFLAGS = $(ROSE_INCLUDES)
feasibility_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
# TESTS
###############################################################################################################################

EXTRA_DIST += marker_function.C
MOSTLYCLEANFILES += marker_function.o sample.o rose_sample.C rose_sample.o sample01
marker_function.o: marker_function.C
	@echo "  CXX32   $@"
	$(CXX) $(CFLAGS32) -c $<

sample01: sample.C marker_function.o markFunctions
	@echo "  GEN     $@"
	./markFunctions -c sample.C
	@echo "  CXX32   $@"
	$(CXX) $(CFLAGS32) -c rose_sample.C
	$(CXX) $(CFLAGS32) -o $@ rose_sample.o marker_function.o
