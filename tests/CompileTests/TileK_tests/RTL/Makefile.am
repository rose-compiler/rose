
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libTileK-RTL.la
libTileK_RTL_la_SOURCES= \
  tilek-rtl.c
libTileK_RTL_la_CFLAGS= \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/tests/CompileTests/TileK_tests \
  $(ROSE_INCLUDES)

libTileK_RTL_includedir=$(prefix)/include/RTL
libTileK_RTL_include_HEADERS=kernel.h


