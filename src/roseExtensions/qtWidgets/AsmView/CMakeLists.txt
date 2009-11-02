
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

##------------ AsmView Makefile.am -----------
#include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 
#
#if ROSE_WITH_ROSEQT
#
#noinst_LTLIBRARIES = libAsmView.la 
#
#
#
#libAsmView_la_SOURCES = AsmTreeNode.cpp\
#                        AsmView.cpp
#
#nodist_libAsmView_la_SOURCES = moc_AsmView.cxx\
#                               ui_ConfigureAsmViewDelegate.h
#
#BUILT_SOURCES = ui_ConfigureAsmViewDelegate.h
#
## Target for installing header 
#headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
#headerTarget_HEADERS = AsmTreeNode.h\
#                       AsmView.h
#
#
#endif
#
#EXTRA_DIST = ConfigureAsmViewDelegate.ui
#
