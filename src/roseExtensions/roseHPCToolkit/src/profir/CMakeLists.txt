
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT Profile IR -- source files
#
#include ../../Make.inc
#
#if ROSE_BUILD_ROSEHPCT
#
#noinst_LTLIBRARIES = libprofir.la
#libprofir_la_SOURCES = profir.cc
#
#else # !ROSE_BUILD_ROSEHPCT
#
#EXTRA_DIST = profir.cc
#
#endif
#
## eof
