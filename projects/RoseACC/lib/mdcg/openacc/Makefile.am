
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-openacc.la

libMDCG_openacc_la_SOURCES= \
  model.cpp
libMDCG_openacc_la_CXXFLAGS= -g -O2 \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(srcdir)/../../../include \
  $(ROSE_INCLUDES)
libMDCG_openacc_la_includedir=$(prefix)/include/KLT
libMDCG_openacc_la_include_HEADERS= \
  $(srcdir)/../../../include/MDCG/OpenACC/model.hpp



