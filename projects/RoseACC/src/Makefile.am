
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=roseacc

roseacc_SOURCES= \
  roseacc.cpp
roseacc_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(SQLITE3_CFLAGS) \
  $(ROSE_INCLUDES)
roseacc_LDFLAGS= \
  $(SQLITE3_LDFLAGS)
roseacc_LDADD= \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(top_builddir)/src/backend/KLT/lib/libKLT.la \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(builddir)/../lib/dlx/openacc/libDLX-openacc.la \
  $(builddir)/../lib/klt/openacc/libKLT-openacc.la \
  $(builddir)/../lib/mdcg/openacc/libMDCG-openacc.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

check-local: clean-local roseacc
	./roseacc -I$(top_srcdir)/../libopenacc/include -c $(top_srcdir)/../tests/101/kernel-openacc.c

clean-local:
	rm -rf host-data.* kernels.cl kernel-openacc.o rose_kernel-openacc.c  versions.db

