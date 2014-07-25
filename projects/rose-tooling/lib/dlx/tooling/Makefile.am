
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-tooling.la

libDLX_tooling_la_SOURCES= \
  language.cpp \
  frontend.cpp \
  clauses.cpp \
  constructs.cpp \
  compiler.cpp
libDLX_tooling_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  $(ROSE_INCLUDES)


