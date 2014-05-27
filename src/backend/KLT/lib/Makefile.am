
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=core

lib_LTLIBRARIES=libKLT.la

libKLT_la_SOURCES= \
  utils.cpp
libKLT_la_CXXFLAGS= -g -O2 \
  -I$(srcdir)/../include \
  $(ROSE_INCLUDES)
libKLT_la_includedir=$(prefix)/include/KLT
libKLT_la_include_HEADERS= \
  $(srcdir)/../include/KLT/utils.hpp



