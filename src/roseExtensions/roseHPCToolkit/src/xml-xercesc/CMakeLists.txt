
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT XML wrappers around Xerces-C library -- sources
#
#include ../../Make.inc
#
##if ROSE_BUILD_ROSEHPCT
##
##noinst_LTLIBRARIES = libxml.la
##libxml_la_SOURCES = elem.cc handler.cc parser.cc util.cc
##
##else # !ROSE_BUILD_ROSEHPCT
#
#EXTRA_DIST = elem.cc handler.cc parser.cc util.cc
#
##endif
#
## eof
