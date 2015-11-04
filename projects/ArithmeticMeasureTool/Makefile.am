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

## Input testcode for your translator
C_TESTCODE= \
    jacobi-v3.c \
    nestedloops.c

TESTS = jacobi-v3.out nestedloops.out
jacobi-v3.out:rose_jacobi-v3.c
	gcc -O3 rose_jacobi-v3.c -o jacobi-v3.out -lm

nestedloops.out:rose_nestedloops.c
	gcc -O3 rose_nestedloops.c -o nestedloops.out -lm

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

C_TESTCODE_RESULTS = $(addprefix rose_, ${C_TESTCODE})
F_TESTCODE_RESULTS = $(addprefix rose_, ${Fortran_Tests})

# test the translator
$(C_TESTCODE_RESULTS):measureTool $(srcdir)/test/$(@:rose_%=%) 
	./measureTool -c -annot $(srcdir)/src/functionSideEffect.annot -I. $(srcdir)/test/$(@:rose_%=%)

$(F_TESTCODE_RESULTS):measureTool $(srcdir)/test/fortran/$(@:rose_%=%) 
	./measureTool -c -annot $(srcdir)/src/functionSideEffect.annot -I. $(srcdir)/test/fortran/$(@:rose_%=%)

ftest: $(F_TESTCODE_RESULTS)

check: $(C_TESTCODE_RESULTS) $(F_TESTCODE_RESULTS)
clean:
	rm -rf $(bin_PROGRAMS) $(builddir)/src/*.o *.o $(builddir)/src/*.a rose_* *.dot *.out *.log *.trs #measureToolReport.txt

EXTRA_DIST=build cleanup.sh test	
