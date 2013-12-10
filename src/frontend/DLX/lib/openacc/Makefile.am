
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-openacc.la

libDLX_openacc_la_SOURCES= \
  language.cpp \
  directives.cpp constructs.cpp clauses.cpp \
  frontend.cpp \
  compiler.cpp
libDLX_openacc_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../../include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
libDLX_openacc_la_includedir=$(prefix)/include/DLX
libDLX_openacc_la_include_HEADERS= \
  $(srcdir)/../../include/DLX/Core/directives.hpp \
  $(srcdir)/../../include/DLX/Core/constructs.hpp \
  $(srcdir)/../../include/DLX/Core/clauses.hpp \
  $(srcdir)/../../include/DLX/Core/frontend.hpp \
  $(srcdir)/../../include/DLX/Core/compiler.hpp \
  $(srcdir)/../../include/DLX/OpenACC/language.hpp


