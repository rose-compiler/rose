
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-sage.la

libMFB_sage_la_SOURCES= \
  mfb-sage.cpp
libMFB_sage_la_CXXFLAGS= \
  -g -I$(srcdir)/../../include \
  $(ROSE_INCLUDES)
libMFB_sage_la_includedir=$(prefix)/include/KLT
libMFB_sage_la_include_HEADERS= \
  $(srcdir)/../../include/MFB/multi-file-builder.hpp \
  $(srcdir)/../../include/MFB/multi-file-builder.tpp \
  $(srcdir)/../../include/MFB/Sage/mfb-sage.hpp \
  $(srcdir)/../../include/MFB/Sage/mfb-sage.tpp

