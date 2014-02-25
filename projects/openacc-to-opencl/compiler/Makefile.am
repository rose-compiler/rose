
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=openacc-to-opencl

openacc_to_opencl_SOURCES= \
  openacc-to-opencl.cpp
openacc_to_opencl_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -g \
  $(ROSE_INCLUDES)
openacc_to_opencl_LDADD = \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/frontend/DLX/lib/openacc/libDLX-openacc.la \
  $(top_builddir)/src/midend/MFB/lib/sage/libMFB-sage.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(top_builddir)/src/backend/KLT/lib/libKLT.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

if ROSE_HAVE_OPENCL_INC
check-local: openacc-to-opencl
	./openacc-to-opencl $(top_srcdir)/../tests/101/101-klt.lt $(top_srcdir)/../libopenacc/ $(OPENCL_INC_DIR)
endif
