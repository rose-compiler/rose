
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-core.la

libKLT_core_la_SOURCES= \
  mfb-klt.cpp \
  kernel.cpp
libKLT_core_la_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)



