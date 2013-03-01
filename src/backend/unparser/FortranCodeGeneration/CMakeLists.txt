
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

set(unparseFortranCodeGeneration_headers
  unparseFortran.h unparseFortran_types.h unparseFortran_modfile.h)
install(FILES  ${unparseFortranCodeGeneration_headers} DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#unparseFortranCodeGeneration_headers = unparseFortran.h unparseFortran_types.h unparseFortran_modfile.h
#unparseFortranCodeGeneration_sources = unparseFortran_expressions.C unparseFortran_statements.C unparseFortran_types.C unparseFortran_modfile.C
#
#include_HEADERS = $(unparseFortranCodeGeneration_headers)
#
#noinst_LTLIBRARIES = libunparseFortran.la
#
#libunparseFortran_la_SOURCES = $(unparseFortranCodeGeneration_sources)
#
#clean-local:
#	rm -rf ii_files ti_files core
