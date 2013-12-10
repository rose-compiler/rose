
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
 
lib_LTLIBRARIES=libROSE-handles.la
 
libROSE_handles_la_SOURCES= \
   transformation-handler.cpp
libROSE_handles_la_CXXFLAGS= \
   -g -I$(srcdir) \
   $(ROSE_INCLUDES)
libROSE_handles_la_includedir=$(prefix)/include
libROSE_handles_la_include_HEADERS= \
  $(srcdir)/handles.hpp \
  $(srcdir)/hooks.hpp \
  $(srcdir)/transformation-handler.hpp

