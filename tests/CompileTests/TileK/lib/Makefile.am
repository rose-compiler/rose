
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES= \
  libDLX-TileK-basic.la       libTileK-RTL-basic.la   \
  libDLX-TileK-threads.la     libTileK-RTL-threads.la \
  libDLX-TileK-accelerator.la
#  libDLX-TileK.la         libMFB-TileK.la          libMDCG-TileK.la          libKLT-TileK.la          libTileK-RTL.la \
#  libDLX-TileK-threads.la libMFB-TileK-threads.la  libMDCG-TileK-threads.la  libKLT-TileK-threads.la  libTileK-RTL-threads.la \
#  libDLX-TileK-acc.la     libMFB-TileK-acc-ocl.la  libMDCG-TileK-acc-ocl.la  libKLT-TileK-acc-ocl.la                          \
#                          libMFB-TileK-acc-cuda.la libMDCG-TileK-acc-cuda.la libKLT-TileK-acc-cuda.la

if ROSE_HAVE_OPENCL_INC
lib_LTLIBRARIES+=libTileK-RTL-opencl.la
endif

if ROSE_HAVE_CUDA_INC
#lib_LTLIBRARIES+=libTileK-RTL-cuda.la
endif

# DLX (frontend)

libDLX_TileK_basic_la_SOURCES= \
  dlx-language.cpp \
  dlx-constructs.cpp \
  dlx-clauses.cpp \
  dlx-frontend.cpp
libDLX_TileK_basic_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

libDLX_TileK_threads_la_SOURCES= \
  dlx-language.cpp \
  dlx-constructs.cpp \
  dlx-clauses.cpp \
  dlx-frontend.cpp
libDLX_TileK_threads_la_CXXFLAGS= \
  -g -DTILEK_THREADS \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

libDLX_TileK_accelerator_la_SOURCES= \
  dlx-language.cpp \
  dlx-constructs.cpp \
  dlx-clauses.cpp \
  dlx-frontend.cpp
libDLX_TileK_accelerator_la_CXXFLAGS= \
  -g -DTILEK_ACCELERATOR \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

# MFB (midend)
#
#libMFB_TileK_la_SOURCES= mfb.cpp
#libMFB_TileK_la_CXXFLAGS= \
#  -g \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMFB_TileK_threads_la_SOURCES= mfb.cpp
#libMFB_TileK_threads_la_CXXFLAGS= \
#  -g -DTILEK_THREADS \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMFB_TileK_acc_ocl_la_SOURCES= mfb.cpp
#libMFB_TileK_acc_ocl_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMFB_TileK_acc_cuda_la_SOURCES= mfb.cpp
#libMFB_TileK_acc_cuda_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
# MDCG (midend)
#
#libMDCG_TileK_la_SOURCES= mdcg.cpp
#libMDCG_TileK_la_CXXFLAGS= \
#  -g \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMDCG_TileK_threads_la_SOURCES= mdcg.cpp
#libMDCG_TileK_threads_la_CXXFLAGS= \
#  -g -DTILEK_THREADS \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMDCG_TileK_acc_ocl_la_SOURCES= mdcg.cpp
#libMDCG_TileK_acc_ocl_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libMDCG_TileK_acc_cuda_la_SOURCES= mdcg.cpp
#libMDCG_TileK_acc_cuda_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
# KLT (backend)
#
#libKLT_TileK_la_SOURCES=klt.cpp
#libKLT_TileK_la_CXXFLAGS= \
#  -g \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libKLT_TileK_threads_la_SOURCES=klt.cpp
#libKLT_TileK_threads_la_CXXFLAGS= \
#  -g -DTILEK_THREADS \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libKLT_TileK_acc_ocl_la_SOURCES=klt.cpp
#libKLT_TileK_acc_ocl_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
#libKLT_TileK_acc_cuda_la_SOURCES=klt.cpp
#libKLT_TileK_acc_cuda_la_CXXFLAGS= \
#  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA \
#  -I$(top_srcdir)/src/frontend/DLX/include \
#  -I$(top_srcdir)/src/midend/MFB/include \
#  -I$(top_srcdir)/src/midend/MDCG/include \
#  -I$(top_srcdir)/src/backend/KLT/include \
#  -I$(srcdir)/../include \
#  $(ROSE_INCLUDES)
#
# Runtime

libTileK_RTL_basic_la_SOURCES=tilek-rtl.c tilek-rtl-basic.c
libTileK_RTL_basic_la_CFLAGS= \
  -g -DTILEK_BASIC \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

libTileK_RTL_threads_la_SOURCES=tilek-rtl.c tilek-rtl-threads.c
libTileK_RTL_threads_la_CFLAGS= \
  -g -DTILEK_THREADS \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

if ROSE_HAVE_OPENCL_INC
libTileK_RTL_opencl_la_SOURCES=tilek-rtl.c tilek-rtl-accelerator.c tilek-rtl-opencl.c
libTileK_RTL_opencl_la_CFLAGS= \
  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL -DCOMPILE_OPENCL_KERNEL_WITH_DEBUG=1 \
  -I@OPENCL_INC_DIR@ \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
endif

if ROSE_HAVE_CUDA_INC
libTileK_RTL_cuda_la_SOURCES=tilek-rtl.c tilek-rtl-accelerator.c tilek-rtl-cuda.c
libTileK_RTL_cuda_la_CFLAGS= \
  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA \
  -I@CUDA_INC_DIR@ \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
endif


