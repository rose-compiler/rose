include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = simpleComputationReverser

simpleComputationReverser_SOURCES = simpleComputationReverser.C

include_HEADERS =

check-local: simpleComputationReverser test1.c
	./simpleComputationReverser -c $(srcdir)/test1.c
	@echo "Test passed"
