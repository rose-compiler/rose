
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

##------------ RoseCodeEdit Makefile.am -----------
#include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 
#
#if ROSE_WITH_ROSEQT
#
#noinst_LTLIBRARIES = libRoseCodeEdit.la 
#
#
#
#libRoseCodeEdit_la_SOURCES = RoseCodeEdit.cpp
#
#
#
#
#
## Target for installing header 
#headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
#headerTarget_HEADERS = RoseCodeEdit.h
#
#
#endif
#
#EXTRA_DIST = 
#
