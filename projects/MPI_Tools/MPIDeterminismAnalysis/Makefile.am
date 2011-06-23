include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MPIDETDIR=$(top_srcdir)/projects/MPI_Tools/MPIDeterminismAnalysis

INCLUDES=$(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
EXTRA_DIST = MpiDeterminismAnalysis.h

bin_PROGRAMS=testMpiDeterminismAnalysis

testMpiDeterminismAnalysis_SOURCES = MpiDeterminismAnalysis.cpp TestMpiDeterminismAnalysis.cpp

test_TARGETS = $(wildcard $(MPIDETDIR)/tests/*.c)

%.mpidet.test: % $(bin_PROGRAMS)
	./testMpiDeterminismAnalysis -I$(MPIDETDIR) $<

check-local: $(patsubst %, %.mpidet.test, $(test_TARGETS))
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/MPI_Tools/MpiDeterminismAnalysis: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.o $(bin_PROGRAMS)
