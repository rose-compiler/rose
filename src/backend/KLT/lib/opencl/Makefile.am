
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=
if ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT
lib_LTLIBRARIES+=libKLT-opencl.la
endif

libKLT_opencl_la_SOURCES= \
  kernel.cpp \
  mfb-klt.cpp \
  iteration-mapper.cpp \
  generator.cpp
libKLT_opencl_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)
libKLT_opencl_la_includedir=$(prefix)/include/KLT
libKLT_opencl_la_include_HEADERS= \
  $(srcdir)/../../include/KLT/OpenCL/kernel.hpp \
  $(srcdir)/../../include/KLT/OpenCL/mfb-klt.hpp


