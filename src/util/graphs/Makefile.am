include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(top_srcdir)/src/util/support -I$(srcdir)/..

CXX_TEMPLATE_REPOSITORY_PATH = .

libgraphsSources = \
   DGBaseGraphImpl.C IDGraphCreate.C  SCCAnalysis.C  TransAnalysis.C  TypedFusion.C

# lib_LIBRARIES = libgraphs.a
# libgraphs_a_SOURCES  = $(libgraphsSources)

if DEV_LIBS
lib_LTLIBRARIES = libgraphs.la
else
noinst_LTLIBRARIES = libgraphs.la
endif
libgraphs_la_SOURCES = $(libgraphsSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
   IDGraphCreate.C \
   BaseGraphCreate.h  GraphScope.h     IDGraphCreate.h \
   TransAnalysis.h    GraphGroup.h       GraphTemplate.h  \
   TypedFusion.h DGBaseGraphImpl.h  GraphInterface.h GraphUtils.h\
   GraphUpdate.h    SCCAnalysis.h GraphDotOutput.h GraphIO.h 
