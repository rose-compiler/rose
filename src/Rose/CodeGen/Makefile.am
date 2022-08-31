include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libRoseCodeGen.la

libRoseCodeGen_la_SOURCES = Driver.C API.C Factory.C
libRoseCodeGen_la_LIBADD = 

libRoseCodeGen_la_public_includedir = $(prefix)/include/Rose
libRoseCodeGen_la_public_include_HEADERS = ../CodeGen.h

libRoseCodeGen_la_includedir = $(prefix)/include/Rose/CodeGen
libRoseCodeGen_la_include_HEADERS = \
  Driver.h Object.h \
  API.h API.txx Factory.h

libRoseCodeGen_factory_txx_includedir = $(prefix)/include/Rose/CodeGen/factory
libRoseCodeGen_factory_txx_include_HEADERS = \
  factory/namespaces.txx factory/classes.txx factory/typedefs.txx factory/variables.txx factory/functions.txx

EXTRA_DIST = CMakeLists.txt

