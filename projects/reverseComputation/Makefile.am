include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = simpleComputationReverser

simpleComputationReverser_SOURCES = simpleComputationReverser.C

EXTRA_DIST = test1.C eventSimulation.h

CLEANFILES = rose_test1.C

include_HEADERS =

check-local: simpleComputationReverser test1.C
	./simpleComputationReverser -c $(srcdir)/test1.C
	@echo "Test passed"
