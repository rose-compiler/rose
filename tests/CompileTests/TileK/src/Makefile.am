
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

INC_PATHS= \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include   \
  -I$(top_srcdir)/src/midend/MDCG/include  \
  -I$(top_srcdir)/src/backend/KLT/include  \
  -I$(srcdir)/../include $(ROSE_INCLUDES)

LIB_PATHS= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/core  \
  -L$(top_builddir)/src/midend/MDCG/lib/klt   \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(top_builddir)/src/backend/KLT/lib/core  \
  -L$(top_builddir)/src/backend/KLT/lib/mdcg  \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH)

KLT_INC_PATH= \
  -DKLT_RTL_INC_PATH=\"$(abs_top_srcdir)/src/backend/KLT\" \
  -DTILEK_RTL_INC_PATH=\"$(abs_top_srcdir)/tests/CompileTests/TileK\"

PATHS= \
  -DKLT_PATH=\"$(abs_top_srcdir)/src/backend/KLT\" \
  -DTILEK_PATH=\"$(abs_top_srcdir)/tests/CompileTests/TileK\"

KLT_LIBS=-lDLX-Core -lMFB-Sage -lMFB-Utils -lMDCG-Core -lMDCG-KLT -lKLT-Core

#########################################

bin_PROGRAMS=tilek-basic tilek-threads tilek-opencl #tilek-cuda

tilek_basic_SOURCES=tilek.cpp
tilek_basic_CXXFLAGS=-g -O0 $(INC_PATHS) $(PATHS) -DTILEK_BASIC
tilek_basic_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/klt    \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/model  \
  -L$(top_builddir)/src/midend/MDCG/lib/tools  \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(top_builddir)/src/backend/KLT/lib/mdcg \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH) \
  -lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT -lKLT-MDCG \
  -lDLX-TileK-basic \
  $(ROSE_LIBS)

tilek_threads_SOURCES=tilek.cpp
tilek_threads_CXXFLAGS=-g -O0 $(INC_PATHS) $(PATHS) -DTILEK_THREADS
tilek_threads_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/klt    \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/model  \
  -L$(top_builddir)/src/midend/MDCG/lib/tools  \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(top_builddir)/src/backend/KLT/lib/mdcg \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH) \
  -lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT -lKLT-MDCG \
  -lDLX-TileK-threads \
  $(ROSE_LIBS)

tilek_opencl_SOURCES=tilek.cpp
tilek_opencl_CXXFLAGS=-g -O0 $(INC_PATHS) $(PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL
tilek_opencl_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/klt    \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/model  \
  -L$(top_builddir)/src/midend/MDCG/lib/tools  \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(top_builddir)/src/backend/KLT/lib/mdcg \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH) \
  -lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT -lKLT-MDCG \
  -lDLX-TileK-accelerator \
  $(ROSE_LIBS)

tilek_cuda_SOURCES=tilek.cpp
tilek_cuda_CXXFLAGS=-g -O0 $(INC_PATHS) $(PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
tilek_cuda_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/klt    \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/model  \
  -L$(top_builddir)/src/midend/MDCG/lib/tools  \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(top_builddir)/src/backend/KLT/lib/mdcg \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH) \
  -lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT -lKLT-MDCG \
  -lDLX-TileK-accelerator \
  $(ROSE_LIBS)

#########################################

