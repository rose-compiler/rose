
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

bin_PROGRAMS=tilek

tilek_SOURCES=tilek.cpp
tilek_CXXFLAGS= \
  -g -O0 \
  -DKLT_RTL_INC_PATH=\"$(abs_top_srcdir)/src/backend/KLT/include/KLT/RTL\" \
  -DTILEK_RTL_INC_PATH=\"$(abs_srcdir)/../include/RTL\" \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
tilek_LDADD= \
  -L$(top_builddir)/src/frontend/DLX/lib/core -lDLX-Core \
  -L$(top_builddir)/src/midend/MFB/lib/sage   -lMFB-Sage \
  -L$(top_builddir)/src/midend/MFB/lib/utils  -lMFB-Utils \
  -L$(top_builddir)/src/midend/MDCG/lib/core  -lMDCG-Core \
  -L$(top_builddir)/src/midend/MDCG/lib/klt   -lMDCG-KLT \
  -L$(top_builddir)/src/backend/KLT/lib/core  -lKLT-Core \
  -L$(builddir)/../lib/ -lDLX-TileK \
  -L$(builddir)/../lib/ -lMFB-TileK \
  -L$(builddir)/../lib/ -lKLT-TileK \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#########################################

