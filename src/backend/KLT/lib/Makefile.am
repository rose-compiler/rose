
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT.la

libKLT_la_SOURCES= \
  mfb-klt.cpp \
  dlx-openacc.cpp \
  runtime-openacc.cpp \
  utils.cpp
libKLT_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)
libKLT_la_includedir=$(prefix)/include/KLT
libKLT_la_include_HEADERS= \
  $(srcdir)/../include/KLT/generator.hpp \
  $(srcdir)/../include/KLT/data.hpp \
  $(srcdir)/../include/KLT/loop-trees.hpp \
  $(srcdir)/../include/KLT/kernel.hpp \
  $(srcdir)/../include/KLT/cg-config.hpp \
  $(srcdir)/../include/KLT/data-flow.hpp \
  $(srcdir)/../include/KLT/iteration-mapper.hpp \
  $(srcdir)/../include/KLT/loop-mapper.hpp \
  $(srcdir)/../include/KLT/mfb-klt.hpp \
  $(srcdir)/../include/KLT/dlx-openacc.hpp \
  $(srcdir)/../include/KLT/utils.hpp



