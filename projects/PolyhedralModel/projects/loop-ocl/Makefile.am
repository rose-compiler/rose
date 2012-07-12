
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = loop_ocl

LDADD = ../../src/libPolyModel.la ../../src/libPolyModelRoseKernel.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

loop_ocl_SOURCES         = \
  loop-ocl.cpp \
  opencl-from-loops.cpp \
  ../utils/rose-utils.cpp
loop_ocl_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../utils/ \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
loop_ocl_LDADD           = $(LDADD)
loop_ocl_includedir      = $(includedir)/polyhedral-model/projects/loop-ocl
loop_ocl_include_HEADERS = opencl-from-loops.hpp

../../src/libPolyModel.la:
	make -C ../../src libPolyModel.la

../../src/libPolyModelRoseKernel.la:
	make -C ../../src libPolyModelRoseKernel.la
