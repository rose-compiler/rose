
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
  -L$(top_builddir)/src/backend/KLT/lib/core  \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH)

KLT_INC_PATH= \
  -DKLT_RTL_INC_PATH=\"$(abs_top_srcdir)/src/backend/KLT\" \
  -DTILEK_RTL_INC_PATH=\"$(abs_top_srcdir)/tests/CompileTests/TileK\"

KLT_LIBS=-lDLX-Core -lMFB-Sage -lMFB-Utils -lMDCG-Core -lMDCG-KLT -lKLT-Core

#########################################

bin_PROGRAMS=tilek tilek-threads tilek-acc-ocl tilek-acc-cuda test-new-klt

tilek_SOURCES=tilek.cpp
tilek_CXXFLAGS=-g -O0 $(INC_PATHS) -DTILEK_BASIC $(KLT_INC_PATH)
tilek_LDADD= $(LIB_PATHS) $(KLT_LIBS) \
  -lDLX-TileK -lMFB-TileK -lMDCG-TileK -lKLT-TileK $(ROSE_LIBS)

tilek_threads_SOURCES=tilek.cpp
tilek_threads_CXXFLAGS=-g -O0 $(INC_PATHS) -DTILEK_THREADS $(KLT_INC_PATH)
tilek_threads_LDADD= $(LIB_PATHS) $(KLT_LIBS) \
  -lDLX-TileK-threads -lMFB-TileK-threads -lMDCG-TileK-threads -lKLT-TileK-threads $(ROSE_LIBS)

tilek_acc_ocl_SOURCES=tilek.cpp
tilek_acc_ocl_CXXFLAGS=-g -O0 $(INC_PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL $(KLT_INC_PATH)
tilek_acc_ocl_LDADD= $(LIB_PATHS) $(KLT_LIBS) \
  -lDLX-TileK-acc -lMFB-TileK-acc-ocl -lMDCG-TileK-acc-ocl -lKLT-TileK-acc-ocl $(ROSE_LIBS)

tilek_acc_cuda_SOURCES=tilek.cpp
tilek_acc_cuda_CXXFLAGS=-g -O0 $(INC_PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA $(KLT_INC_PATH)
tilek_acc_cuda_LDADD= $(LIB_PATHS) $(KLT_LIBS) \
  -lDLX-TileK-acc -lMFB-TileK-acc-cuda -lMDCG-TileK-acc-cuda -lKLT-TileK-acc-cuda $(ROSE_LIBS)

#########################################

test_new_klt_SOURCES=test-new-klt.cpp
test_new_klt_CXXFLAGS=-g -O0 $(INC_PATHS) $(KLT_INC_PATH)
test_new_klt_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   \
  -L$(top_builddir)/src/midend/MFB/lib/klt    \
  -L$(top_builddir)/src/midend/MFB/lib/utils  \
  -L$(top_builddir)/src/midend/MDCG/lib/core  \
  -L$(top_builddir)/src/backend/KLT/lib/      \
  -L$(builddir)/../lib/ $(LIBS_WITH_RPATH) \
  -lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Core -lKLT \
  -lDLX-TileK-acc \
  $(ROSE_LIBS)

#########################################

