
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

set(unparseFormat_headers unparse_format.h unparseFormatHelp.h)
install(FILES  ${unparseFormat_headers} DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#unparseFormat_sources = $(unparseFormat_headers:.h=.C)
#
#include_HEADERS = $(unparseFormat_headers)
#
#noinst_LTLIBRARIES = libunparseFormatSupport.la
#
#libunparseFormatSupport_la_SOURCES = $(unparseFormat_sources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
#
