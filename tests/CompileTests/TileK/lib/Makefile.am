
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES= \
  libDLX-TileK-basic.la       libTileK-RTL-basic.la   \
  libDLX-TileK-threads.la     libTileK-RTL-threads.la \
  libDLX-TileK-accelerator.la

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

# Runtime

libTileK_RTL_basic_la_SOURCES=tilek-rtl.c tilek-rtl-basic.c
libTileK_RTL_basic_la_CFLAGS= \
  -g -DTILEK_BASIC \
  -I$(top_srcdir)/src/midend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

libTileK_RTL_threads_la_SOURCES=tilek-rtl.c tilek-rtl-threads.c
libTileK_RTL_threads_la_CFLAGS= \
  -g -DTILEK_THREADS \
  -I$(top_srcdir)/src/midend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

if ROSE_HAVE_OPENCL_INC
libTileK_RTL_opencl_la_SOURCES=tilek-rtl.c tilek-rtl-accelerator.c tilek-rtl-opencl.c
libTileK_RTL_opencl_la_CFLAGS= \
  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL -DCOMPILE_OPENCL_KERNEL_WITH_DEBUG=1 \
  -I@OPENCL_INC_DIR@ \
  -I$(top_srcdir)/src/midend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
endif

if ROSE_HAVE_CUDA_INC
libTileK_RTL_cuda_la_SOURCES=tilek-rtl.c tilek-rtl-accelerator.c tilek-rtl-cuda.c
libTileK_RTL_cuda_la_CFLAGS= \
  -g -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA \
  -I@CUDA_INC_DIR@ \
  -I$(top_srcdir)/src/midend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
endif


