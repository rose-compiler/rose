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

bin_PROGRAMS = measureTool codeInstrumentor
measureTool_SOURCES = measureTool.cpp 
codeInstrumentor_SOURCES = codeInstrumentor.cpp

$(builddir)/src/libarithemeticMeasurement.a:
	make -C $(builddir)/src

#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
measureTool_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(builddir)/src/libarithemeticMeasurement.a 
codeInstrumentor_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

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
   test1.c \
   bzip2.c \
   switch.c \
   gzip.c \
   oggenc.c

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
# test for code instrumentor---------------------
#Generate the loop count info. file , using the translator's loop_counting mode
instrumentor_lib.o: $(srcdir)/src/instrumentor_lib.c
	gcc -c -o $@ $^
instrumentor_lib.a: instrumentor_lib.o
	ar r $@ $^
bt-loop-count.log: codeInstrumentor
	./codeInstrumentor -c -I$(srcdir)/src $(srcdir)/test/bt_single.c -exe_mode=loop_counting -file-for-loop-count bt-loop-count.log -o rose_bt_single_v.c
# generate the instrumented code, using the loop count info. file and the translator's instrumenting mode (default one)
rose_bt_single.c: bt-loop-count.log  $(srcdir)/test/bt_single.c
	./codeInstrumentor -c -I$(srcdir)/src $(srcdir)/test/bt_single.c -file-for-loop-count bt-loop-count.log -file-for-loop-iteration-count bt-iter-count.log
#compile the instrumented code: avoid using rose translator as the linker!! conflicting binary analysis when .a file is present!!
rose_bt_single.out: rose_bt_single.c instrumentor_lib.a
	gcc $< instrumentor_lib.a -lm -o $@
# In the end, we want the information about loops' iteration count	
bt-iter-count.log:rose_bt_single.out
	./rose_bt_single.out
ftest: $(F_TESTCODE_RESULTS)

check: $(C_TESTCODE_RESULTS) $(F_TESTCODE_RESULTS) $(C_Analysis_TESTCODE_RESULTS) bt-iter-count.log
clean:
	rm -rf $(bin_PROGRAMS) $(builddir)/src/*.o *.o $(builddir)/src/*.a rose_* *.dot *.out *.log *.trs ai_tool_report.txt bt-loop-count.log bt-iter-count.log instrumentor_lib.a

EXTRA_DIST=build cleanup.sh test	
