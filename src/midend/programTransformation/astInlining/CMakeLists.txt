
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############

install(FILES  replaceExpressionWithStatement.h inlinerSupport.h inliner.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#libastinliningSources = \
#   inliner.C inlinerSupport.C isPotentiallyModified.C \
#   replaceExpressionWithStatement.C 
#
## lib_LTLIBRARIES = libastinlining.a
## libastinlining_a_SOURCES  = $(libastinliningSources)
#
#noinst_LTLIBRARIES = libastinlining.la
#libastinlining_la_SOURCES = $(libastinliningSources)
#
#include_HEADERS = replaceExpressionWithStatement.h inlinerSupport.h inliner.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
