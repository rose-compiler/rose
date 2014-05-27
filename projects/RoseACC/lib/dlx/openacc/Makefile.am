
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libDLX-openacc.la

libDLX_openacc_la_SOURCES= \
  language.cpp \
  directives.cpp constructs.cpp clauses.cpp \
  frontend.cpp \
  compiler.cpp
libDLX_openacc_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../../../include \
  $(ROSE_INCLUDES)
libDLX_openacc_la_includedir=$(prefix)/include/DLX
libDLX_openacc_la_include_HEADERS= \
  $(top_srcdir)/src/frontend/DLX/include/DLX/Core/directives.hpp \
  $(top_srcdir)/src/frontend/DLX/include/DLX/Core/constructs.hpp \
  $(top_srcdir)/src/frontend/DLX/include/DLX/Core/clauses.hpp \
  $(top_srcdir)/src/frontend/DLX/include/DLX/Core/frontend.hpp \
  $(top_srcdir)/src/frontend/DLX/include/DLX/Core/compiler.hpp \
  $(srcdir)/../../../include/DLX/OpenACC/language.hpp


