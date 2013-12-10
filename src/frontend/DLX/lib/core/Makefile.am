
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-core.la

libDLX_core_la_SOURCES= \
  frontend.cpp parser.cpp
libDLX_core_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
libDLX_core_la_includedir=$(prefix)/include/DLX
libDLX_core_la_include_HEADERS= \
  $(srcdir)/../../include/DLX/Core/directives.hpp \
  $(srcdir)/../../include/DLX/Core/constructs.hpp \
  $(srcdir)/../../include/DLX/Core/clauses.hpp \
  $(srcdir)/../../include/DLX/Core/frontend.hpp \
  $(srcdir)/../../include/DLX/Core/compiler.hpp


