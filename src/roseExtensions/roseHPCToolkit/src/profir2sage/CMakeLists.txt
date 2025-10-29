
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT Profile IR--to-Sage IR conversion -- source files
#
#include ../../Make.inc
#
#if ROSE_BUILD_ROSEHPCT
#
#noinst_LTLIBRARIES = libprofir2sage.la
#libprofir2sage_la_SOURCES = attach.cc
#
#else # !ROSE_BUILD_ROSEHPCT
#
#EXTRA_DIST = attach.cc
#
#endif
#
## eof
