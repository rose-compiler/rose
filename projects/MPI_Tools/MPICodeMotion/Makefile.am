include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES=$(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/staticInterproceduralSlicing -I$(top_srcdir)/src/midend/programAnalysis/CallGraphAnalysis

bin_PROGRAMS=mpiCodeMotion

mpiCodeMotion_SOURCES = CallGraphQuery.C MotionInformation.C MPINonBlockingCodeMotion.C testMPINonBlockingCodeMotion.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

clean-local:
	rm -f *.o mpiCodeMotion rose_*.C

EXTRA_DIST = CallGraphQuery.h MotionInformation.h  MPINonBlockingCodeMotion.h

#include_HEADERS=DependenceGraph.h CallGraph.h CallGraphQuery.h MotionInformation.h MPINonBlockingCodeMotion.h
