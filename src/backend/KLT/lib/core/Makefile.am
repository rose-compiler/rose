
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-Core.la

libKLT_Core_la_SOURCES= \
  mfb-klt.cpp \
  languages.cpp \
  utils.cpp
libKLT_Core_la_CXXFLAGS= \
  -g -O2 \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)



