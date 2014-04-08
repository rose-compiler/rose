
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=looptree-to-opencl

looptree_to_opencl_SOURCES= \
  looptree-to-opencl.cpp \
  mdcg_openacc.cpp \
  klt_itmap.cpp
looptree_to_opencl_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  $(SQLITE3_CFLAGS) \
  $(ROSE_INCLUDES)
looptree_to_opencl_LDFLAGS= \
  $(SQLITE3_LDFLAGS)
looptree_to_opencl_LDADD= \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/frontend/DLX/lib/openacc/libDLX-openacc.la \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(top_builddir)/src/backend/KLT/lib/libKLT.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

