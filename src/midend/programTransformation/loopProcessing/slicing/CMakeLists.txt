
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface)


########### next target ###############


########### install files ###############

install(FILES  CompSliceDepGraph.h  CompSliceImpl.h      DynamicCompSlice.h CompSliceObserver.h                     CompSlice.h          CompSliceLocality.h  CompSliceRegistry.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(srcdir)/../driver -I$(srcdir)/../computation -I$(srcdir)/../depGraph -I$(srcdir)/../depInfo -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libslicingSources = \
#   CompSlice.C  DynamicCompSlice.C CompSliceDepGraph.C  CompSliceLocality.C  DependenceHoisting.C
#
## lib_LTLIBRARIES = libslicing.a
## libslicing_a_SOURCES  = $(libslicingSources)
#
#noinst_LTLIBRARIES = libslicing.la
#libslicing_la_SOURCES = $(libslicingSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  CompSliceDepGraph.h  CompSliceImpl.h      DynamicCompSlice.h CompSliceObserver.h \
#                   CompSlice.h          CompSliceLocality.h  CompSliceRegistry.h
#
#
#
#
#
#
#
#
