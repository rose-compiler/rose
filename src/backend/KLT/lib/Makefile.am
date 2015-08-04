
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=#core rtl

lib_LTLIBRARIES=libKLT.la

libKLT_la_SOURCES= \
  generator.cpp \
  looptree.cpp \
  kernel.cpp \
  descriptor.cpp \
  api.cpp \
  utils.cpp
libKLT_la_CXXFLAGS= \
  -g -O2 \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)

