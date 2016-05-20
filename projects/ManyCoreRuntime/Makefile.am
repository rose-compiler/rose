include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = docs

# AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_CPPFLAGS = 

# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -lz
# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = jacobi testPermutations hypercube

MANYCORE_FILES = $(srcdir)/highresTimer.C $(srcdir)/core.h $(srcdir)/multicore.h $(srcdir)/core.C $(srcdir)/multicore.C

jacobi_SOURCES = $(srcdir)/jacobi.C $(srcdir)/highresTimer.C
jacobi_DEPENDENCIES = $(srcdir)/jacobi.C $(srcdir)/highresTimer.C $(MANYCORE_FILES)

testPermutations_SOURCES = $(srcdir)/testPermutations.C $(srcdir)/highresTimer.C
testPermutations_DEPENDENCIES = $(MANYCORE_FILES)

hypercube_SOURCES = hypercube.C


# CXXFLAGS += -fopenmp -O2
# CXXFLAGS += 

# ROSE configure script has a check for the existence of numa.h header and triggers this automake conditional.
LDADD = 
if !OS_MACOSX
   LDADD += -lrt
endif
if ROSE_USE_LIBNUMA
   LDADD += -lnuma
endif

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DEPENDENT_SOURCE_FILES = $(srcdir)/core.h $(srcdir)/multicore.h $(srcdir)/core.C $(srcdir)/multicore.C
# valgrind: jacobi $(DEPENDENT_SOURCE_FILES)

test: jacobi 
	./jacobi

permutations: testPermutations 
	./testPermutations

test_hypercube: hypercube
	./hypercube

valgrind: jacobi
	$(VALGRIND) ./jacobi

valgrind_permutations: testPermutations 
	$(VALGRIND) ./testPermutations

EXTRA_DIST = $(srcdir)/core.h $(srcdir)/multicore.h highresTimer.h \
             $(srcdir)/core.C $(srcdir)/multicore.C highresTimer.C

CLEANFILES = 

include_HEADERS =

check-local:
if !OS_32BIT
#	DQ (11/2/2011): This appears to fail for 32bit system, we will debug this separately.
	./testPermutations
endif
	@echo "*******************************************************************************************"
	@echo "****** ROSE/projects/ManyCoreRuntime: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************"
