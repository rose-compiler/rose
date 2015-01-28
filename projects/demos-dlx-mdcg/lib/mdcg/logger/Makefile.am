
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-logger.la

libMDCG_logger_la_SOURCES= \
  static-initializer.cpp
libMDCG_logger_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  $(ROSE_INCLUDES)


