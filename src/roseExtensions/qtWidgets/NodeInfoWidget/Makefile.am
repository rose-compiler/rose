#------------ NodeInfoWidget Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libNodeInfoWidget.la 



libNodeInfoWidget_la_SOURCES = NodeInfoWidget.cpp

nodist_libNodeInfoWidget_la_SOURCES = moc_NodeInfoWidget.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = NodeInfoWidget.h


endif
