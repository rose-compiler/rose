include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libRoseAST.la

libRoseAST_la_SOURCES = graphviz.C fixupTraversal.C io/merge.C io/link.C io/delete.C io/share.C io/load.C checker/consistency.C checker/checker.C
libRoseAST_la_LIBADD = 

libRoseAST_la_includedir = $(prefix)/include/Rose/AST
libRoseAST_la_include_HEADERS = cmdline.h io.h checker.h fixupTraversal.h

EXTRA_DIST = CMakeLists.txt

