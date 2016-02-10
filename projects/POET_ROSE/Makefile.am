include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = rose_pcg
rose_pcg_SOURCES=  main.cpp eval_ext_rose.cpp

poet_dir=${top_srcdir}/src/3rdPartyLibraries/POET
comp=diff

SUBDIRS = manycore-stencil test

EXTRA_DIST=Cpp_ROSE.incl 


check:
	./rose_pcg -L$(poet_dir)/lib -L$(srcdir) -pin="$(srcdir)/test/dgemm_test.C" -pout=cfg.out $(srcdir)/test/CFG.pt
	$(comp) cfg.out $(srcdir)/test/cfg.out
	./rose_pcg $(debug) -L$(srcdir) -L$(poet_dir)/lib -L$(srcdir)/manycore-stencil -L$(srcdir)/manycore-stencil/include -pin="$(srcdir)/manycore-stencil/benchmarks/testPermutations.C" -pout="out.C" $(srcdir)/manycore-stencil/stencil-gen.pt 
	$(comp) out.C $(srcdir)/manycore-stencil/benchmarks/testPermutations-save.C
	./rose_pcg $(debug) -L$(srcdir) -L$(srcdir)/manycore-stencil -L$(srcdir)/manycore-stencil/include -L$(poet_dir)/lib -pin="$(srcdir)/manycore-stencil/benchmarks/tmp.C" -pout="out.C" $(srcdir)/manycore-stencil/stencil-gen.pt 
	$(comp) out.C $(srcdir)/manycore-stencil/benchmarks/tmp-save.C
	./rose_pcg $(debug) -L$(srcdir)/manycore-stencil -L$(srcdir)/manycore-stencil/include -L$(poet_dir)/lib -L$(srcdir) -pin=$(srcdir)/manycore-stencil/benchmarks/"test6cell.C" -pout="out.C" $(srcdir)/manycore-stencil/stencil-gen.pt
	$(comp) out.C $(srcdir)/manycore-stencil/benchmarks/test6cell-save.C
	./rose_pcg $(debug) -pcp_num=8 -L$(srcdir)/manycore-stencil -L$(srcdir)/manycore-stencil/include -L$(poet_dir)/lib -L$(srcdir) -pin=$(srcdir)/manycore-stencil/benchmarks/"test6cell.C" -pout="out.C" $(srcdir)/manycore-stencil/stencil-gen.pt
	$(comp) out.C $(srcdir)/manycore-stencil/benchmarks/test6cell_rmem-save.C

AM_YFLAGS=-d -v

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/midend/astUtil/astInterface  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/3rdPartyLibraries/POET

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

compile_now: 
	g++ out.C -I $(srcdir)/manycore-stencil/include $(srcdir)/manycore-stencil/multicore.C $(srcdir)/manycore-stencil/core.C $(srcdir)/manycore-stencil/highresTimer.C -lrt -fopenmp

now: 
	./rose_pcg -L$(srcdir)/test $(srcdir)/test/opt_uj.pt
