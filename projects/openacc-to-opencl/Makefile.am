
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=modules

bin_PROGRAMS=openacc-to-opencl

openacc_to_opencl_SOURCES= \
  openacc-to-opencl.cpp
openacc_to_opencl_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
openacc_to_opencl_LDADD = \
  $(top_builddir)/src/util/Handles/libROSE-handles.la \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/frontend/DLX/lib/openacc/libDLX-openacc.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

check-local: openacc-to-opencl

