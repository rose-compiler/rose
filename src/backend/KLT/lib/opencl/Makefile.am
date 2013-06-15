
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=
if ROSE_USE_OPENCL_SUPPORT
lib_LTLIBRARIES+=libKLT-opencl.la
endif

libKLT_opencl_la_SOURCES= \
  kernel.cpp \
  generator.cpp
libKLT_opencl_la_CXXFLAGS= \
  -g -I$(srcdir)/../../include \
  $(ROSE_INCLUDES)
libKLT_opencl_la_LIBADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
libKLT_opencl_la_includedir=$(prefix)/include/KLT
libKLT_opencl_la_include_HEADERS= \
  $(srcdir)/../../include/KLT/OpenCL/kernel.hpp \
  $(srcdir)/../../include/KLT/OpenCL/generator.hpp


