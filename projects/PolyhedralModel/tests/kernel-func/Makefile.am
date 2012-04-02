
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = kernel-function-polygraph-extractor

LDADD = ../../src/libPolyModel.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

kernel_function_polygraph_extractor_SOURCES         = \
  kernel-function-polygraph-extractor.cpp \
  PolyhedralKernel.cpp
kernel_function_polygraph_extractor_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
kernel_function_polygraph_extractor_LDADD           = $(LDADD)
kernel_function_polygraph_extractor_bindir          = $(includedir)/polyhedral-model/kernel-func
kernel_function_polygraph_extractor_includedir      = $(includedir)/polyhedral-model/kernel-func
kernel_function_polygraph_extractor_include_HEADERS = PolyhedralKernel.hpp

lib_LTLIBRARIES = libPolyhedralKernel.la

libPolyhedralKernel_la_SOURCES = \
  PolyhedralKernel.cpp
libPolyhedralKernel_la_CXXFLAGS = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)

../../src/libPolyModel.la:
	make -C ../../src

