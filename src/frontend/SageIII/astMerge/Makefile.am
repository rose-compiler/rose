include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
GENERATED_SOURCE  = ../Cxx_Grammar.C ../Cxx_GrammarTreeTraversalSuccessorContainer.C ../Cxx_GrammarRTI.C
GENERATED_OBJECTS = ../Cxx_Grammar.o ../Cxx_GrammarTreeTraversalSuccessorContainer.o ../Cxx_GrammarRTI.o

$(GENERATED_SOURCE):
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

noinst_LTLIBRARIES = libastMerge.la

libastMerge_la_SOURCES = merge.C fixupTraversal.C link-function.C link-variable.C link-class.C link-namespace.C delete.C share.C load.C
libastMerge_la_LIBADD = 
libastMerge_la_DEPENDENCIES = $(GENERATED_SOURCE)

pkginclude_HEADERS = merge.h fixupTraversal.h link.h delete.h share.h load.h

EXTRA_DIST = CMakeLists.txt
