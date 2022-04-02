include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libRoseAST.la

libRoseAST_la_SOURCES = checker.C graphviz.C merge.C fixupTraversal.C link.C delete.C share.C load.C consistency.C
libRoseAST_la_LIBADD = 

libRoseAST_la_includedir = $(prefix)/include/Rose/AST
libRoseAST_la_include_HEADERS = cmdline.h checker.h graphviz.h  merge.h fixupTraversal.h

EXTRA_DIST = CMakeLists.txt

