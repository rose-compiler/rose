
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-logger.la

libDLX_logger_la_SOURCES= \
  language.cpp \
  frontend.cpp \
  clauses.cpp \
  constructs.cpp \
  compiler.cpp
libDLX_logger_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  $(ROSE_INCLUDES)


