#------------ PropertyTreeWidget Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libPropertyTreeWidget.la 



libPropertyTreeWidget_la_SOURCES = PropertyTreeModel.cpp\
                                   PropertyTreeWidget.cpp

nodist_libPropertyTreeWidget_la_SOURCES = moc_PropertyTreeModel.cxx\
                                          moc_PropertyTreeWidget.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = PropertyTreeModel.h\
                       PropertyTreeWidget.h


endif
