
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-sequential.la

libKLT_sequential_la_SOURCES= \
  kernel.cpp \
  mfb-klt.cpp \
  generator.cpp \
  iteration-mapper.cpp
libKLT_sequential_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)
libKLT_sequential_la_includedir=$(prefix)/include/KLT
libKLT_sequential_la_include_HEADERS= \
  $(srcdir)/../../include/KLT/Sequential/kernel.hpp \
  $(srcdir)/../../include/KLT/Core/generator.hpp
  


