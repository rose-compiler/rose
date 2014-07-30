
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG.la

libMDCG_la_SOURCES= \
  model.cpp \
  model-builder.cpp \
  model-element.cpp \
  static-initializer.cpp
libMDCG_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)


