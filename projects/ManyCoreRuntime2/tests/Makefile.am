include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#AUTOMAKE_OPTIONS = subdir-objects
RUNTIMEDIR = $(top_srcdir)/projects/ManyCoreRuntime2/runtime
LIBDIR = $(top_builddir)/projects/ManyCoreRuntime2/runtime
#INCLUDES = $(ROSE_INCLUDES)
INCLUDES = $(RUNTIMEDIR) 

# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lz
# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = stencil jacobi jacobihaloDetached jacobihaloAttached\
jacobi3D jacobihaloDetached3D jacobihaloAttached3D jacobiOMP2D jacobiOMP3D\
hypercube testPermutations

BUILT_SOURCES = highresTimer.C 

stencil_SOURCES = 6cell.C highresTimer.C 

jacobi_SOURCES = jacobi.C highresTimer.C

jacobi3D_SOURCES = jacobi-3D.C highresTimer.C

jacobihaloDetached_SOURCES = jacobi-halo-Detached.C highresTimer.C

jacobihaloDetached3D_SOURCES = jacobi-3D-halo-Detached.C highresTimer.C

jacobihaloAttached_SOURCES = jacobi-halo-Attached.C highresTimer.C

jacobihaloAttached3D_SOURCES = jacobi-3D-halo-Attached.C highresTimer.C

jacobiOMP2D_SOURCES = jacobi-OMP-heap-2D.C highresTimer.C

jacobiOMP3D_SOURCES = jacobi-OMP-heap-3D.C highresTimer.C

testPermutations_SOURCES = testPermutations.C 

hypercube_SOURCES = hypercube.C


CXXFLAGS += -fopenmp -O3 -I$(INCLUDES) -L$(LIBDIR)

# ROSE configure script has a check for the existence of numa.h header and triggers this automake conditional.
LDADD = -lmulticore 
if !OS_MACOSX
   LDADD += -lrt
endif
if ROSE_USE_LIBNUMA
   LDADD += -lnuma 
   CXXFLAGS += -lnuma -DHAVE_NUMA_H=1
endif

#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
#VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DEPENDENT_SOURCE_FILES = $(srcdir)/core.h $(srcdir)/multicore.h $(srcdir)/core.C $(srcdir)/multicore.C
# valgrind: jacobi $(DEPENDENT_SOURCE_FILES)

#test: jacobi 
#	./jacobi

#permutations: testPermutations 
#	./testPermutations

##test_hypercube: hypercube
#	./hypercube

#valgrind: jacobi
#	$(VALGRIND) ./jacobi

#valgrind_permutations: testPermutations 
#	$(VALGRIND) ./testPermutations

CLEANFILES = 

include_HEADERS =

check-local:
if !OS_32BIT
#	DQ (11/2/2011): This appears to fail for 32bit system, we will debug this separately.
#	./testPermutations
endif
	@echo "*******************************************************************************************"
	@echo "****** ROSE/projects/ManyCoreRuntime: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************"
