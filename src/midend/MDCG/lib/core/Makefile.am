
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-Core.la

libMDCG_Core_la_SOURCES= \
  model.cpp \
  model-builder.cpp \
  model-element.cpp \
  model-class.cpp \
  static-initializer.cpp
libMDCG_Core_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)


