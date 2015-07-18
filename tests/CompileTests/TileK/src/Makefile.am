
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

#########################################

bin_PROGRAMS=tilek tilek-threads

tilek_SOURCES=tilek.cpp
tilek_CXXFLAGS=-g -O0 $(INC_PATHS) \
  -DKLT_RTL_INC_PATH=\"$(abs_top_srcdir)/src/backend/KLT/include/KLT/RTL\" \
  -DTILEK_RTL_INC_PATH=\"$(abs_srcdir)/../include/RTL\"
tilek_LDADD= $(LIB_PATHS) \
  -lDLX-Core -lMFB-Sage -lMFB-Utils -lMDCG-Core -lMDCG-KLT -lKLT-Core \
  -lDLX-TileK -lMFB-TileK -lMDCG-TileK -lKLT-TileK $(ROSE_LIBS)

tilek_threads_SOURCES=tilek.cpp
tilek_threads_CXXFLAGS=-g -O0 $(INC_PATHS) -DTILEK_THREADS \
  -DKLT_RTL_INC_PATH=\"$(abs_top_srcdir)/src/backend/KLT/include/KLT/RTL\" \
  -DTILEK_RTL_INC_PATH=\"$(abs_srcdir)/../include/RTL\"
tilek_threads_LDADD= $(LIB_PATHS) \
  -lDLX-Core -lMFB-Sage -lMFB-Utils -lMDCG-Core -lMDCG-KLT -lKLT-Core \
  -lDLX-TileK-threads -lMFB-TileK-threads -lMDCG-TileK-threads -lKLT-TileK-threads $(ROSE_LIBS)

#########################################

