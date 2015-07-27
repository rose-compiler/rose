
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libKLT-RTL.la

CFLAGS=

libKLT_RTL_la_SOURCES= \
  build-loop-context.c \
  context.c
libKLT_RTL_la_CFLAGS= -g -O0 \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)



