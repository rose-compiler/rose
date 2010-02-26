
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  BaseGraphCreate.h  GraphScope.h     VirtualGraphCreate.h     TransAnalysis.h    GraphGroup.h   MultiGraphCreate.h     TypedFusion.h DGBaseGraphImpl.h  GraphAccess.h GraphUtils.h    GraphUpdate.h    SCCAnalysis.h GraphDotOutput.h GraphIO.h     MultiGraphObserve.h DESTINATION ${INCLUDE_INSTALL_DIR})


#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(top_srcdir)/src/util/support -I$(srcdir)/..
#
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libgraphsSources = \
#   DGBaseGraphImpl.C SCCAnalysis.C  TransAnalysis.C  TypedFusion.C 
#
## lib_LIBRARIES = libgraphs.a
## libgraphs_a_SOURCES  = $(libgraphsSources)
#
#noinst_LTLIBRARIES = libgraphs.la
#libgraphs_la_SOURCES = $(libgraphsSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = \
#   BaseGraphCreate.h  GraphScope.h     VirtualGraphCreate.h \
#   TransAnalysis.h    GraphGroup.h   MultiGraphCreate.h \
#   TypedFusion.h DGBaseGraphImpl.h  GraphAccess.h GraphUtils.h\
#   GraphUpdate.h    SCCAnalysis.h GraphDotOutput.h GraphIO.h \
#   MultiGraphObserve.h
