
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-TileK.la libMFB-TileK.la libKLT-TileK.la libTileK-RTL.la

# DLX (frontend)

libDLX_TileK_la_SOURCES= \
  dlx-language.cpp \
  dlx-constructs.cpp \
  dlx-clauses.cpp \
  dlx-frontend.cpp
libDLX_TileK_la_CXXFLAGS= \
  -g -O0 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

# MFB (midend)

libMFB_TileK_la_SOURCES= mfb.cpp
libMFB_TileK_la_CXXFLAGS= \
  -g -I$(top_srcdir)/src/frontend/DLX/include \
  -g -I$(top_srcdir)/src/midend/MFB/include \
  -g -I$(top_srcdir)/src/midend/MDCG/include \
  -g -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

# KLT (backend)

libKLT_TileK_la_SOURCES=klt.cpp
libKLT_TileK_la_CXXFLAGS= \
  -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)

# Runtime

libTileK_RTL_la_SOURCES=rtl.c
libTileK_RTL_la_CFLAGS= \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)


