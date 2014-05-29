
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=looptree-to-opencl openacc-parser

looptree_to_opencl_SOURCES= \
  looptree-to-opencl.cpp
looptree_to_opencl_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(SQLITE3_CFLAGS) \
  $(ROSE_INCLUDES)
looptree_to_opencl_LDFLAGS= \
  $(SQLITE3_LDFLAGS)
looptree_to_opencl_LDADD= \
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

openacc_parser_SOURCES= \
  openacc-parser.cpp
openacc_parser_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(SQLITE3_CFLAGS) \
  $(ROSE_INCLUDES)
openacc_parser_LDFLAGS= \
  $(SQLITE3_LDFLAGS)
openacc_parser_LDADD= \
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

check-local: check-looptree-to-opencl check-openacc-parser

check-looptree-to-opencl: looptree-to-opencl

check-openacc-parser: openacc-parser
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -c $(top_srcdir)/../tests/101/kernel-openacc.c
