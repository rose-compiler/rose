
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-openacc.la

libKLT_openacc_la_SOURCES= \
  dlx-openacc.cpp \
  runtime-openacc.cpp \
  mfb-openacc.cpp \
  klt-openacc.cpp \
  iteration-mapper.cpp \
  code-generation.cpp
libKLT_openacc_la_CXXFLAGS= -g -O2 \
  -I$(srcdir)/../../../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  $(ROSE_INCLUDES)
libKLT_openacc_la_includedir=$(prefix)/include/KLT
libKLT_openacc_la_include_HEADERS= \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/generator.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/data.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/loop-trees.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/kernel.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/cg-config.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/data-flow.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/iteration-mapper.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/loop-mapper.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/Core/mfb-klt.hpp \
  $(top_srcdir)/src/backend/KLT/include/KLT/utils.hpp \
  $(srcdir)/../../../include/KLT/OpenACC/dlx-openacc.hpp



