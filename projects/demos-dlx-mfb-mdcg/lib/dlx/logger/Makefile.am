
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-logger.la

libDLX_logger_la_SOURCES= \
  language.cpp \
  frontend.cpp \
  clauses.cpp \
  constructs.cpp
libDLX_logger_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  $(ROSE_INCLUDES)


