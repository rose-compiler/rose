
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT XML-to-Profile IR conversion -- source files
#
#include ../../Make.inc
#
#if ROSE_BUILD_ROSEHPCT
#
#noinst_LTLIBRARIES = libxml2profir.la
#libxml2profir_la_SOURCES = convert.cc factory.cc process.cc query.cc
#
#else # !ROSE_BUILD_ROSEHPCT
#
#EXTRA_DIST = convert.cc factory.cc process.cc query.cc
#
#endif
#
## eof
