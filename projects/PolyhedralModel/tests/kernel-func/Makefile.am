
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = polygrapher loop_ocl pocc_post_processor

LDADD = ../../src/libPolyModel.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

polygrapher_SOURCES         = \
  polygrapher.cpp \
  PolyhedralKernel.cpp \
  parsetree.cpp \
  polygraph.cpp
polygrapher_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  -I/home/tristan/opt/ppcg/include \
  $(ROSE_INCLUDES)
polygrapher_LDADD           = $(LDADD) -L/home/tristan/opt/ppcg/lib -lisl
polygrapher_bindir          = $(includedir)/polyhedral-model/kernel-func
polygrapher_includedir      = $(includedir)/polyhedral-model/kernel-func
polygrapher_include_HEADERS = PolyhedralKernel.hpp parsetree.hpp polygraph.hpp

loop_ocl_SOURCES         = \
  PolyhedralKernel.cpp \
  loop-ocl.cpp \
  opencl-from-loops.cpp \
  rose-utils.cpp
loop_ocl_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
loop_ocl_LDADD           = $(LDADD)
loop_ocl_bindir          = $(includedir)/polyhedral-model/kernel-func
loop_ocl_includedir      = $(includedir)/polyhedral-model/kernel-func
loop_ocl_include_HEADERS = PolyhedralKernel.hpp opencl-from-loops.hpp rose-utils.hpp

pocc_post_processor_SOURCES         = \
  PolyhedralKernel.cpp \
  pocc-post-processor.cpp \
  rose-utils.cpp
pocc_post_processor_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
pocc_post_processor_LDADD           = $(LDADD)
pocc_post_processor_bindir          = $(includedir)/polyhedral-model/kernel-func
pocc_post_processor_includedir      = $(includedir)/polyhedral-model/kernel-func
pocc_post_processor_include_HEADERS = PolyhedralKernel.hpp

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

