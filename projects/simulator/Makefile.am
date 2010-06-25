include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if !ROSE_MPI

if ROSE_USE_ASSEMBLY_SEMANTICS

INCLUDES = $(ROSE_INCLUDES)

LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


bin_PROGRAMS = simulator 


simulator_SOURCES = emulator.C simulate.C linuxcalls.C simulate.h memory.h elfload.C
simulator_CPPFLAGS = $(ROSE_INCLUDES)
simulator_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local: 

test-dep: 

ppc-check:

CLEANFILES = 

endif

endif

EXTRA_DIST = test.c


