
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  $(unparsePythonCodeGeneration_headers) DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#unparsePythonCodeGeneration_headers = unparsePython.h
#
#unparsePythonCodeGeneration_sources = unparsePython.C
#
#include_HEADERS = $(unparsePythonCodeGeneration_headers)
#
#noinst_LTLIBRARIES = libunparsePython.la
#
#libunparsePython_la_SOURCES = $(unparsePythonCodeGeneration_sources)
#
#clean-local:
#	rm -rf ii_files ti_files core
#
