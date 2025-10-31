
include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/util/support)


########### next target ###############


########### install files ###############

install(FILES  AnnotCollect.h      AnnotExpr.h  AnnotStmt.h           OperatorDescriptors.h     AnnotDescriptors.h  AnnotIO.h    OperatorAnnotation.h  TypeAnnotation.h     CPPAnnotation.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../astInterface -I$(srcdir)/../symbolicVal -I$(srcdir)/../astSupport -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libannotationSources = \
#   AnnotCollect.C      AnnotExpr.C  OperatorAnnotation.C   TypeAnnotation.C \
#   AnnotDescriptors.C  AnnotIO.C    OperatorDescriptors.C
#
## lib_LTLIBRARIES = libannotation.a
## libannotation_a_SOURCES = $(libannotationSources)
#
#noinst_LTLIBRARIES = libannotation.la
#libannotation_la_SOURCES = $(libannotationSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = \
#   AnnotCollect.h      AnnotExpr.h  AnnotStmt.h           OperatorDescriptors.h \
#   AnnotDescriptors.h  AnnotIO.h    OperatorAnnotation.h  TypeAnnotation.h \
#   CPPAnnotation.h
