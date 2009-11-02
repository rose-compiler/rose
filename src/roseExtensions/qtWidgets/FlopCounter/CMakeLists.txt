
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

##------------ FlopCounter Makefile.am -----------
#include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 
#
#if ROSE_WITH_ROSEQT
#
#noinst_LTLIBRARIES = libFlopCounter.la 
#
#
#
#libFlopCounter_la_SOURCES = FlopCounter.cpp\
#                            Flops.cpp
#
#
#
#
#
## Target for installing header 
#headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
#headerTarget_HEADERS = FlopCounter.h\
#                       Flops.h
#
#
#endif
#
#EXTRA_DIST = 
#
