include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES=$(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST = MpiDeterminismAnalysis.h

bin_PROGRAMS=testMpiDeterminismAnalysis

testMpiDeterminismAnalysis_SOURCES = MpiDeterminismAnalysis.cpp TestMpiDeterminismAnalysis.cpp

clean-local:
	rm -f *.o testMpiDeterminismAnalysis
