#------------ PropertyTreeWidget Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libPropertyTreeWidget.la 



libPropertyTreeWidget_la_SOURCES = PropertyTreeModel.cpp\
                                   PropertyTreeWidget.cpp

nodist_libPropertyTreeWidget_la_SOURCES = moc_PropertyTreeModel.cxx\
                                          moc_PropertyTreeWidget.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = PropertyTreeModel.h\
                       PropertyTreeWidget.h


endif

EXTRA_DIST = CMakeLists.txt 

