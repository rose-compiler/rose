
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-TileK.la

CXXFLAGS=
AM_CXXFLAGS=

libDLX_TileK_la_SOURCES= \
  language.cpp \
  constructs.cpp \
  clauses.cpp \
  frontend.cpp
libDLX_TileK_la_CXXFLAGS= \
  -g -O0 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  $(ROSE_INCLUDES)

