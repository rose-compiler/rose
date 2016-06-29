# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#DEBUG_LEVEL     = -g -O0
#EXTRA_CCFLAGS   = -Wall
#CXXFLAGS        = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
#CCFLAGS         = $(CXXFLAGS)

# search path for .m4 macros
#ACLOCAL_AMFLAGS = -I ./config

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/src #-I$(ROSE_INSTALL)/include/rose -I$(BOOST_INSTALL)/include 

SUBDIRS = src . 

bin_PROGRAMS = measureTool
measureTool_SOURCES = measureTool.cpp 

$(builddir)/src/libarithemeticMeasurement.a:
	make -C $(builddir)/src

#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
measureTool_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(builddir)/src/libarithemeticMeasurement.a 

## Test input testcode with manual instrumentation already
C_TESTCODE= \
    jacobi-v3.c \
    nestedloops.c \
    test1-annot.c \
    test2-nested-annot.c

TESTS = rose_jacobi-v3.out \
        rose_nestedloops.out \
	rose_test1-annot.out \
	rose_test2-nested-annot.out

# test files for static counting mode. No changes to the input file at all
C_Analysis_Only_TESTCODE = \
   jacobi.c \
   test-1d-1.c \
   test1.c

# loop001.cpp hello.cpp
# compilation error for some f files
# InterpFace.f   
#   InterpF_correct.f
Fortran_Tests = \
   AMRPoissonOpF.f \
   AMRPoissonOpF_GSMCAMRPOP.f \
   EdgeToCellF_EDGETOCELLMAX.f \
   InterpF_001.f \
   QuadCFInterpF.f \
   CellToEdgeF.f \
   EdgeToCellF.f \
   InterpF_002.f \
   AverageCodimF.f90

# rose_test.c will be the target
C_Analysis_TESTCODE_RESULTS = $(addprefix rose_, ${C_Analysis_Only_TESTCODE})

C_TESTCODE_RESULTS = $(addprefix rose_, ${C_TESTCODE})
F_TESTCODE_RESULTS = $(addprefix rose_, ${Fortran_Tests})

check_PROGRAM = ${C_TESTCODE_RESULTS:.c=.out}

# this does not work somehow ??
#TESTS = ${check_PROGRAM}
%.out: %.c
	gcc $^ -o $@ -lm
#jacobi-v3.out:rose_jacobi-v3.c
#	gcc -O3 rose_jacobi-v3.c -o jacobi-v3.out -lm
#
#nestedloops.out:rose_nestedloops.c
#	gcc -O3 rose_nestedloops.c -o nestedloops.out -lm

$(C_Analysis_TESTCODE_RESULTS):measureTool $(srcdir)/test/$(@:rose_%=%) 
	./measureTool -c -static-counting-only -use-algorithm-v2 -annot $(srcdir)/src/functionSideEffect.annot -I. $(srcdir)/test/$(@:rose_%=%)

# test the translator
$(C_TESTCODE_RESULTS):measureTool $(srcdir)/test/$(@:rose_%=%) 
	./measureTool -c -annot $(srcdir)/src/functionSideEffect.annot -I. $(srcdir)/test/$(@:rose_%=%)

$(F_TESTCODE_RESULTS):measureTool $(srcdir)/test/fortran/$(@:rose_%=%) 
	./measureTool -c -annot $(srcdir)/src/functionSideEffect.annot -I. $(srcdir)/test/fortran/$(@:rose_%=%)

ftest: $(F_TESTCODE_RESULTS)

check: $(C_TESTCODE_RESULTS) $(F_TESTCODE_RESULTS) $(C_Analysis_TESTCODE_RESULTS)
clean:
	rm -rf $(bin_PROGRAMS) $(builddir)/src/*.o *.o $(builddir)/src/*.a rose_* *.dot *.out *.log *.trs ai_tool_report.txt

EXTRA_DIST=build cleanup.sh test	
