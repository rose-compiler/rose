include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = parallelSTLQuery OpenMPPreprocessor

OpenMPPreprocessor_SOURCES = OpenMPPreprocessor.C
OpenMPPreprocessor_DEPENDENCIES = OpenMPPreprocessor.o $(ROSE_LIBS_WITH_PATH)

# Makefile info for query to decide if we can parallelize iterations
# over user-defined containers.
parallelSTLQuery_SOURCES = parallelSTLQuery.C
parallelSTLQuery_DEPENDENCIES = parallelSTLQuery.o $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# changes to the templates in ../AST_RewriteMechanism/rewriteTreeTraversal.C should force
# several files using those templates to be recompiled.  Express this explicitly:
OpenMPPreprocessor.o: $(top_builddir)/src/midend/astRewriteMechanism/rewriteTreeTraversal.o

EXTRA_DIST = $(TESTCODES) intone.h
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf

check-local:
	rm -f testOpenMP_01.o
	$(MAKE) $(TEST_Objects)

TESTCODES = 

#    testOpenMP_01.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS =

NANOS_AM_CPPFLAGS = /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/include

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN OpenMPPreprocessor
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./OpenMPPreprocessor -I$(srcdir) $(ROSE_FLAGS) $(AM_CPPFLAGS) $(NANOS_AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

example_support.o: $(srcdir)/example_support.c
	gcc -c $(srcdir)/example_support.c

example.o: $(srcdir)/example.C
	gcc -I. -DLINUXPENTIUM -c $(srcdir)/example.C

example: example.o example_support.o
	gcc -o example example.o example_support.o /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libnthsup.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libintone.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libnthreads.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libqt.a

#	g++ -o example example.o example_support.o -lnthsup -lintone -lnthreads -lqt
#	g++ -o example example.o example_support.o -L/home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -lnthsup -lintone -lnthreads -lqt

#	ld -o example example.o example_support.o /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libnthsup.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libintone.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libnthreads.a /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib/libqt.a -lc

# Working example of how to compile and link (however it does seg fault)
nanosBug_working: nanosBug.C
	gcc -I. -DLINUXPENTIUM -c $(srcdir)/nanosBug.C
	gcc -I. -o nanosBug $(srcdir)/nanosBug.C -Wl,-rpath /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -L/home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -lintone -lnthreads -lqt

nanosBug: nanosBug.C
	gcc -I. -DLINUXPENTIUM -c $(srcdir)/nanosBug.C
	gcc -I. -o nanosBug $(srcdir)/nanosBug.C -Wl,-rpath /home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -L/home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -lnthsup -lintone -lnthreads -lqt

#	gcc -I. -o nanosBug $(srcdir)/nanosBug.C -Wl,-rpath=/home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -lnthsup -lintone -lnthreads -lqt
#	gcc -o nanosBug nanosBug.o -L/home/dquinlan/ROSE/OpenMP_Nanos/nanos/local/lib -lnthsup -lintone -lnthreads -lqt

rose_parallelSTLQuery_testFile.C: parallelSTLQuery $(srcdir)/parallelSTLQuery_testFile.C
	cp $(srcdir)/parallelSTLQuery_testFile.C parallelSTLQuery_testFile.C
	$(VALGRIND) ./parallelSTLQuery -c parallelSTLQuery_testFile.C
	rm -rf rose_parallelSTLQuery_testFile.C













