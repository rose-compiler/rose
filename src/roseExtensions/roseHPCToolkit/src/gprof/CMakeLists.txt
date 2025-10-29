
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

## ROSE-HPCT Sage IR interface -- source files
#
#include ../../Make.inc
#
#if ROSE_BUILD_ROSEHPCT
#
#noinst_LTLIBRARIES = libgprof.la
#libgprof_la_SOURCES = gprof_info.cc
#
#else # !ROSE_BUILD_ROSEHPCT
#
#EXTRA_DIST = gprof_info.cc
#
#endif
#
## eof
