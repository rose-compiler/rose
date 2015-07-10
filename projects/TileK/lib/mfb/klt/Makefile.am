
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-KLT-TileK.la

CXXFLAGS=
AM_CXXFLAGS=

libMFB_KLT_TileK_la_SOURCES= \
  tilek.cpp
libMFB_KLT_TileK_la_CXXFLAGS= \
  -g -O0 \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)

