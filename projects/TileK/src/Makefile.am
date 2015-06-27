
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

check-local: rose_foo.c rose_bar.c rose_test_1.c

clean-local:
	rm -f rose_*.c *-kernel.c *.o compiler

#########################################

bin_PROGRAMS=tilek

tilek_SOURCES= \
  compiler.cpp
tilek_CXXFLAGS= \
  -g -O0 \
  -DTILEK_INC_PATH=\"$(srcdir)/../include\" \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)
tilek_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib \
  -L$(top_builddir)/src/midend/MFB/lib/sage \
  -L$(top_builddir)/src/midend/MDCG/lib \
  -L$(top_builddir)/src/backend/KLT/lib \
  -L$(top_builddir)/src/backend/KLT/lib/core \
  -lDLX-core -lMFB-sage -lMDCG -lKLT -lKLT-core \
  $(builddir)/../lib/dlx/tilek/libDLX-TileK.la \
  $(builddir)/../lib/klt/tilek/libKLT-TileK.la \
  $(builddir)/../lib/mdcg/tilek/libMDCG-TileK.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#########################################

rose_foo.c: $(srcdir)/../examples/foo.c $(builddir)/tilek
	./tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/../examples/foo.c

rose_bar.c: $(srcdir)/../examples/bar.c $(builddir)/tilek
	./tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/../examples/bar.c

rose_test_1.c: $(srcdir)/../examples/test_1.c $(builddir)/tilek
	./tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/../examples/test_1.c

#########################################

$(builddir)/../lib/dlx/tilek/libDLX-TileK.la:
	make -C $(builddir)/../lib/dlx/tilek libDLX-TileK.la

$(builddir)/../lib/klt/tilek/libKLT-TileK.la:
	make -C $(builddir)/../lib/klt/tilek libKLT-TileK.la

#########################################

