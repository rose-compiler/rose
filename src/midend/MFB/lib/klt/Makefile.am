
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-KLT.la

libMFB_KLT_la_SOURCES= \
  locals.cpp
libMFB_KLT_la_CXXFLAGS= \
  -g -I$(top_srcdir)/src/frontend/DLX/include \
  -g -I$(top_srcdir)/src/midend/MFB/include \
  -g -I$(top_srcdir)/src/midend/MDCG/include \
  -g -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)

