
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-TileK.la

CXXFLAGS=
AM_CXXFLAGS=

libKLT_TileK_la_SOURCES= \
  tilek.cpp
libKLT_TileK_la_CXXFLAGS= \
  -g -O0 \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)

