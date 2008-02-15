include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../depGraph -I$(srcdir)/../depInfo -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface  -I$(top_srcdir)/src/midend/astUtil/symbolicVal

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libcomputationSources = \
   LoopTree.C   LoopTreeDummyNode.C  LoopTreeShadow.C LoopTreeLocality.C\
   LoopTreeDepComp.C  DepCompTransform.C LoopTreeTransform.C

# lib_LTLIBRARIES = libcomputation.a
# libcomputation_a_SOURCES  = $(libcomputationSources)

if DEV_LIBS
lib_LTLIBRARIES = libcomputation.la
else
noinst_LTLIBRARIES = libcomputation.la
endif
libcomputation_la_SOURCES = $(libcomputationSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
   LoopTreeDummyNode.h  LoopTreeShadow.h LoopTreeLocality.h \
   LoopTreeBuild.h    LoopTree.h           LoopTreeObserver.h  LoopTreeTransform.h \
   LoopTreeDepComp.h  LoopTreeHoldNode.h    DepCompTransform.h 









