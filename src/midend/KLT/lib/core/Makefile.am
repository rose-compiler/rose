
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-Core.la

libKLT_Core_la_SOURCES= \
  generator.cpp \
  looptree.cpp \
  kernel.cpp \
  descriptor.cpp \
  api.cpp \
  utils.cpp
libKLT_Core_la_CXXFLAGS= \
  -g -O2 \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/KLT/include \
  $(ROSE_INCLUDES)

