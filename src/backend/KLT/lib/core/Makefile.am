
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-core.la

libKLT_core_la_SOURCES= \
  loop-trees.cpp \
  kernel.cpp \
  data.cpp \
  generator.cpp \
  cg-config.cpp \
  data-flow.cpp \
  loop-selector.cpp \
  mfb-klt.cpp \
  utils.cpp
libKLT_core_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/backend/MFB/include \
  $(ROSE_INCLUDES)
libKLT_core_la_includedir=$(prefix)/include/KLT
libKLT_core_la_include_HEADERS= \
  $(srcdir)/../../include/KLT/Core/generator.hpp \
  $(srcdir)/../../include/KLT/Core/data.hpp \
  $(srcdir)/../../include/KLT/Core/loop-trees.hpp \
  $(srcdir)/../../include/KLT/Core/kernel.hpp


