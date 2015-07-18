
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG-KLT.la

libMDCG_KLT_la_SOURCES=klt-c-c.cpp
libMDCG_KLT_la_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)


