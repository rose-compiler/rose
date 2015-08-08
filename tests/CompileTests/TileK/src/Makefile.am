
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

DLX_REL_PATH=src/frontend/DLX
MFB_REL_PATH=src/midend/MFB
MDCG_REL_PATH=src/midend/MDCG
KLT_REL_PATH=src/midend/KLT
TILEK_REL_PATH=tests/CompileTests/TileK

KLT_INC_PATHS= \
  -I$(top_srcdir)/$(DLX_REL_PATH)/include  \
  -I$(top_srcdir)/$(MFB_REL_PATH)/include  \
  -I$(top_srcdir)/$(MDCG_REL_PATH)/include \
  -I$(top_srcdir)/$(KLT_REL_PATH)/include
  

INC_PATHS= $(KLT_INC_PATHS) -I$(srcdir)/../include $(ROSE_INCLUDES)

KLT_LIB_PATHS= \
  -L$(top_builddir)/$(DLX_REL_PATH)/lib/core \
  -L$(top_builddir)/$(MFB_REL_PATH)/lib/sage -L$(top_builddir)/$(MFB_REL_PATH)/lib/klt -L$(top_builddir)/$(MFB_REL_PATH)/lib/utils \
  -L$(top_builddir)/$(MDCG_REL_PATH)/lib/model -L$(top_builddir)/$(MDCG_REL_PATH)/lib/tools \
  -L$(top_builddir)/$(KLT_REL_PATH)/lib/core   -L$(top_builddir)/$(KLT_REL_PATH)/lib/mdcg

LIB_PATHS=$(KLT_LIB_PATHS) -L$(builddir)/../lib/ $(LIBS_WITH_RPATH)

RTL_INC_PATHS=-DKLT_PATH=\"$(abs_top_srcdir)/$(KLT_REL_PATH)\" -DTILEK_PATH=\"$(abs_top_srcdir)/$(TILEK_REL_PATH)\"

KLT_LIBS=-lDLX-Core -lMFB-Sage -lMFB-KLT -lMFB-Utils -lMDCG-Model -lMDCG-Tools -lKLT-Core -lKLT-MDCG

#########################################

bin_PROGRAMS=tilek-basic tilek-threads tilek-opencl #tilek-cuda

tilek_basic_SOURCES=tilek.cpp
tilek_basic_CXXFLAGS=-g -O0 $(INC_PATHS) $(RTL_INC_PATHS) -DTILEK_BASIC
tilek_basic_LDADD=$(LIB_PATHS) $(KLT_LIBS) $(ROSE_LIBS) -lDLX-TileK-basic

tilek_threads_SOURCES=tilek.cpp
tilek_threads_CXXFLAGS=-g -O0 $(INC_PATHS) $(RTL_INC_PATHS) -DTILEK_THREADS
tilek_threads_LDADD= $(LIB_PATHS) $(KLT_LIBS) $(ROSE_LIBS) -lDLX-TileK-threads

tilek_opencl_SOURCES=tilek.cpp
tilek_opencl_CXXFLAGS=-g -O0 $(INC_PATHS) $(RTL_INC_PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL
tilek_opencl_LDADD=$(LIB_PATHS) $(KLT_LIBS) $(ROSE_LIBS) -lDLX-TileK-accelerator

#tilek_cuda_SOURCES=tilek.cpp
#tilek_cuda_CXXFLAGS=-g -O0 $(INC_PATHS) $(RTL_INC_PATHS) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
#tilek_cuda_LDADD=$(LIB_PATHS) $(KLT_LIBS) $(ROSE_LIBS) $(KLT_LIBS) -lDLX-TileK-accelerator

#########################################

