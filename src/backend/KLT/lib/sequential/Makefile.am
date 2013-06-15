
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-sequential.la

libKLT_sequential_la_SOURCES= \
  kernel.cpp \
  generator.cpp
libKLT_sequential_la_CXXFLAGS= \
  -g -I$(srcdir)/../../include \
  $(ROSE_INCLUDES)
libKLT_sequential_la_LIBADD= \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
libKLT_sequential_la_includedir=$(prefix)/include/KLT
libKLT_sequential_la_include_HEADERS= \
  $(srcdir)/../../include/KLT/Sequential/kernel.hpp \
  $(srcdir)/../../include/KLT/Sequential/generator.hpp
  


