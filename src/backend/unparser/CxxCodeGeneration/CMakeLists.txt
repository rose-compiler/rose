
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

set(unparseCxxCodeGeneration_headers unparseCxx.h unparseCxx_types.h)
install(FILES  ${unparseCxxCodeGeneration_headers} DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#unparseCxxCodeGeneration_headers = unparseCxx.h unparseCxx_types.h
#
#unparseCxxCodeGeneration_sources = unparseCxx_expressions.C unparseCxx_statements.C unparseCxx_types.C
#
#include_HEADERS = $(unparseCxxCodeGeneration_headers)
#
#noinst_LTLIBRARIES = libunparseCxx.la
#
#libunparseCxx_la_SOURCES = $(unparseCxxCodeGeneration_sources)
#
#clean-local:
#	rm -rf ii_files ti_files core
#
