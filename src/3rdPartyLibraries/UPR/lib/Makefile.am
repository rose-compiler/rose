include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libUPR-core.la
if ROSE_HAVE_CUDA_INC
  lib_LTLIBRARIES+=libUPR-CUDA.la
endif
if ROSE_HAVE_OPENCL_INC
  lib_LTLIBRARIES+=libUPR-OpenCL.la
endif
if ROSE_HAVE_XOMP_INC
  lib_LTLIBRARIES+=libUPR-Xomp.la
endif

libUPR_core_la_SOURCES=runtime.cpp
libUPR_core_la_CXXFLAGS= \
  -g -I$(srcdir)/../include
libUPR_core_la_includedir=$(prefix)/include/UPR
libUPR_core_la_include_HEADERS=$(srcdir)/../include/UPR/runtime.hpp

libUPR_CUDA_la_SOURCES=cuda-runtime.cpp
libUPR_CUDA_la_CXXFLAGS= \
  -g -I$(srcdir)/../include
libUPR_CUDA_la_includedir=$(prefix)/include/UPR
libUPR_CUDA_la_include_HEADERS=$(srcdir)/../include/UPR/cuda-runtime.hpp

libUPR_OpenCL_la_SOURCES=opencl-runtime.cpp
libUPR_OpenCL_la_CXXFLAGS= \
  -I$(OPENCL_INC_DIR) \
  -g -I$(srcdir)/../include
libUPR_OpenCL_la_includedir=$(prefix)/include/UPR
libUPR_OpenCL_la_include_HEADERS=$(srcdir)/../include/UPR/opencl-runtime.hpp

libUPR_Xomp_la_SOURCES=xomp-runtime.cpp
libUPR_Xomp_la_CXXFLAGS= \
  -g -I$(srcdir)/../include
libUPR_Xomp_la_includedir=$(prefix)/include/UPR
libUPR_Xomp_la_include_HEADERS=$(srcdir)/../include/UPR/xomp-runtime.hpp

